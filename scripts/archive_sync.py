#!/usr/bin/env python3
"""Plan and import the owner's missing contest sources using the qwq snapshot.

The loopback intake accepts source already read through a normal browser. It does
not extract cookies, execute submissions, or work around an OJ access check.
"""
from __future__ import annotations

import argparse
from contextlib import contextmanager
import datetime as dt
import fcntl
from functools import wraps
import hashlib
from http.server import BaseHTTPRequestHandler, HTTPServer
import json
import os
from pathlib import Path
import re
import secrets
import threading
import time
from urllib.error import HTTPError
from urllib.parse import parse_qs, urlparse
from urllib.request import Request, urlopen

from archive_plan import build_plan, language_extension, validate_segment
from archive_catalog import apply_contest_mappings

ROOT = Path(__file__).resolve().parents[1]
DASHBOARD_URL = 'https://ojflare.lucius7.dev/data/dashboard.json'
HANDLE = 'Lucius7'
STATE = ROOT / '.archive'
MANIFEST = ROOT / 'archive' / 'submissions.json'
_THREAD_LOCK = threading.RLock()
_LOCK_DEPTH = 0


def now():
    return dt.datetime.now(dt.timezone.utc).isoformat(timespec='seconds')


def read_json(file, default=None):
    return json.loads(file.read_text(encoding='utf-8')) if file.exists() else default


def safe_file(relative):
    path = Path(relative)
    if path.is_absolute() or not path.parts:
        raise ValueError('A relative archive path is required')
    current = ROOT
    for part in path.parts:
        if part != '.archive':
            validate_segment(part)
        current = current / part
        if current.is_symlink():
            raise ValueError('Symlinks are not archive targets')
    if not current.resolve().is_relative_to(ROOT.resolve()):
        raise ValueError('Archive path escaped the repository')
    return current


def sync_directory(directory):
    descriptor = os.open(directory, os.O_RDONLY)
    try:
        os.fsync(descriptor)
    finally:
        os.close(descriptor)


@contextmanager
def repository_lock():
    """Serialize all repository mutations, including separate server processes."""
    global _LOCK_DEPTH
    with _THREAD_LOCK:
        if _LOCK_DEPTH:
            _LOCK_DEPTH += 1
            try:
                yield
            finally:
                _LOCK_DEPTH -= 1
            return
        lock = safe_file(STATE.relative_to(ROOT) / 'process.lock')
        lock.parent.mkdir(parents=True, exist_ok=True)
        with lock.open('a+b') as stream:
            fcntl.flock(stream, fcntl.LOCK_EX)
            _LOCK_DEPTH = 1
            try:
                yield
            finally:
                _LOCK_DEPTH = 0
                fcntl.flock(stream, fcntl.LOCK_UN)


def locked(function):
    @wraps(function)
    def call(*args, **kwargs):
        with repository_lock():
            return function(*args, **kwargs)
    return call


def atomic_json(file, value):
    safe_file(file.relative_to(ROOT))
    file.parent.mkdir(parents=True, exist_ok=True)
    temporary = file.parent / (file.name + '.' + secrets.token_hex(6) + '.tmp')
    try:
        with temporary.open('x', encoding='utf-8') as stream:
            json.dump(value, stream, ensure_ascii=False, indent=2)
            stream.write('\n')
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(temporary, file)
        sync_directory(file.parent)
    finally:
        temporary.unlink(missing_ok=True)


def load_manifest():
    safe_file(MANIFEST.relative_to(ROOT))
    manifest = read_json(MANIFEST, {'schemaVersion': 1, 'handle': HANDLE, 'submissions': []})
    if (not isinstance(manifest, dict) or manifest.get('schemaVersion') != 1
            or manifest.get('handle') != HANDLE or not isinstance(manifest.get('submissions'), list)
            or any(not isinstance(row, dict) for row in manifest['submissions'])):
        raise ValueError('Invalid archive manifest')
    return manifest


def has_receipt(manifest, receipt):
    matches = [row for row in manifest['submissions'] if row.get('path') == receipt['path']]
    if matches and (len(matches) != 1 or matches[0] != receipt):
        raise ValueError('Existing manifest receipt conflicts with pending import')
    return bool(matches)


def matches_source(path, receipt):
    if not path.is_file() or path.stat().st_size != receipt['bytes']:
        return False
    return hashlib.sha256(path.read_bytes()).hexdigest() == receipt['sha256']


@locked
def recover_import():
    """Finish one journaled import; incomplete temporary data is never published."""
    journal = safe_file(STATE.relative_to(ROOT) / 'pending-import.json')
    pending = read_json(journal)
    if pending is None:
        return None
    if not isinstance(pending, dict) or pending.get('schemaVersion') != 1:
        raise ValueError('Invalid pending import journal')
    receipt = pending['receipt']
    if (not isinstance(receipt, dict) or type(receipt.get('bytes')) is not int
            or not 0 < receipt['bytes'] <= 2_000_000
            or not re.fullmatch(r'[0-9a-f]{64}', receipt.get('sha256', ''))):
        raise ValueError('Invalid pending source fingerprint')
    transaction = pending.get('transaction', '')
    if not re.fullmatch(r'[0-9a-f]{24}', transaction):
        raise ValueError('Invalid pending transaction ID')
    target = safe_file(receipt['path'])
    temporary = safe_file(pending['temporary'])
    if temporary != target.with_name(target.name + '.archive-' + transaction + '.tmp'):
        raise ValueError('Invalid pending temporary path')
    manifest = load_manifest()
    recorded = has_receipt(manifest, receipt)
    if not target.exists():
        if recorded:
            raise ValueError('Recorded archive source is missing; pending import retained')
        if not temporary.exists() or not matches_source(temporary, receipt):
            # This transaction never published. Remove only its private temp file.
            temporary.unlink(missing_ok=True)
            journal.unlink()
            sync_directory(journal.parent)
            return 'aborted'
        os.link(temporary, target)  # Atomic publication; fails if target exists.
        sync_directory(target.parent)
    if not matches_source(target, receipt):
        raise ValueError('Pending archive target differs from verified source; preserved')
    if not recorded:
        # The surviving hard link proves this transaction created the target.
        if not temporary.exists() or not os.path.samefile(temporary, target):
            raise ValueError('Existing target was not published by this transaction; preserved')
        manifest['submissions'].append(receipt)
        atomic_json(MANIFEST, manifest)
    temporary.unlink(missing_ok=True)
    sync_directory(target.parent)
    journal.unlink()
    sync_directory(journal.parent)
    return 'imported'


@locked
def publish_source(body, receipt):
    """Persist intent before writing, then publish only a complete verified file."""
    journal = safe_file(STATE.relative_to(ROOT) / 'pending-import.json')
    if journal.exists():
        raise ValueError('Recover the previous import before publishing another')
    target = safe_file(receipt['path'])
    if target.exists():
        raise FileExistsError('Archive target already exists')
    target.parent.mkdir(parents=True, exist_ok=True)
    transaction = secrets.token_hex(12)
    temporary = target.with_name(target.name + '.archive-' + transaction + '.tmp')
    atomic_json(journal, {'schemaVersion': 1, 'transaction': transaction, 'receipt': receipt,
                          'temporary': temporary.relative_to(ROOT).as_posix()})
    with temporary.open('xb') as stream:
        stream.write(body)
        stream.flush()
        os.fsync(stream.fileno())
    sync_directory(temporary.parent)
    if recover_import() != 'imported':
        raise ValueError('Temporary source failed its integrity check')


class Client:
    def __init__(self):
        self.last = {}

    def get(self, url):
        host = urlparse(url).hostname
        interval = 2.2 if host == 'codeforces.com' else 1.1
        time.sleep(max(0, interval - (time.monotonic() - self.last.get(host, 0))))
        self.last[host] = time.monotonic()
        request = Request(url, headers={'Accept': 'application/json', 'User-Agent': 'Lucius7-Archive/1.0 (+https://github.com/xw7qwq/solutions)'})
        try:
            with urlopen(request, timeout=30) as response:
                if urlparse(response.url).hostname != host:
                    raise ValueError('Unexpected metadata redirect')
                body = response.read(12_000_001)
                if len(body) > 12_000_000:
                    raise ValueError('Metadata response too large')
            return json.loads(body)
        except HTTPError as error:
            # Stop on access/rate limits: no retries, alternate domains or cookies.
            raise RuntimeError(f'{host}: HTTP {error.code}; stopped') from None


def metadata_history(client, platform):
    records, cursor = {}, 0 if platform == 'atcoder' else 1
    while True:
        if platform == 'atcoder':
            page = client.get(f'https://kenkoooo.com/atcoder/atcoder-api/v3/user/submissions?user={HANDLE}&from_second={cursor}')
            size = 500
        else:
            payload = client.get(f'https://codeforces.com/api/user.status?handle={HANDLE}&from={cursor}&count=1000')
            if payload.get('status') != 'OK':
                raise ValueError('Codeforces metadata was not OK')
            page, size = payload['result'], 1000
        if not isinstance(page, list):
            raise ValueError('Invalid submission page')
        previous = len(records)
        for item in page:
            if platform == 'atcoder':
                if item['user_id'].casefold() != HANDLE.casefold():
                    raise ValueError('AtCoder returned another user')
                contest, sid = item['contest_id'], item['id']
                record = dict(platform=platform, id=sid, contestId='atcoder:' + contest,
                              problemId='atcoder:' + item['problem_id'], epoch=item['epoch_second'],
                              language=item['language'], verdict=item['result'], handle=HANDLE,
                              codeSize=item.get('length'), url=f'https://atcoder.jp/contests/{contest}/submissions/{sid}')
            else:
                if not any(m['handle'].casefold() == HANDLE.casefold() for m in item['author']['members']):
                    raise ValueError('Codeforces returned another user')
                contest, sid = item.get('contestId'), item['id']
                if contest is None:
                    continue  # No contest association; outside this import's scope.
                index = item['problem']['index']
                section = 'gym' if int(contest) >= 100000 else 'contest'
                record = dict(platform=platform, id=sid, contestId=f'codeforces:{contest}',
                              problemId=f'codeforces:{contest}:{index}', epoch=item['creationTimeSeconds'],
                              language=item['programmingLanguage'], verdict=item.get('verdict', 'UNKNOWN'),
                              handle=HANDLE, participantType=item['author']['participantType'],
                              url=f'https://codeforces.com/{section}/{contest}/submission/{sid}')
            records[record['id']] = record
        print(f'{platform}: {len(records)} submission records', flush=True)
        if len(page) < size:
            break
        if len(records) == previous:
            raise ValueError('Submission pagination stopped advancing')
        if platform == 'atcoder':
            next_cursor = max(item['epoch_second'] for item in page)
            if next_cursor <= cursor:
                raise ValueError('AtCoder cursor stopped advancing')
            cursor = next_cursor
        else:
            cursor += 990
    return list(records.values())


def histories():
    result = []
    for platform in ('atcoder', 'codeforces', 'qoj'):
        payload = read_json(STATE / f'{platform}-history.json', [])
        result.extend(payload['records'] if isinstance(payload, dict) else payload)
    return result


@locked
def current_plan():
    recover_import()
    dashboard = read_json(STATE / 'dashboard.json')
    if dashboard is None:
        raise ValueError('Run the plan command first')
    history = histories()
    catalog = read_json(ROOT / 'archive' / 'contest-mappings.json')
    mapped_dashboard = apply_contest_mappings(dashboard, history, catalog)
    plan = build_plan(mapped_dashboard, ROOT, history or None)
    plan['metadataErrors'] = read_json(STATE / 'metadata-errors.json', {})
    plan['plannedAt'] = now()
    atomic_json(STATE / 'plan.json', plan)
    return plan


@locked
def prepare(args):
    recover_import()
    client = Client()
    dashboard = read_json(Path(args.dashboard)) if args.dashboard else client.get(DASHBOARD_URL)
    if dashboard.get('schemaVersion') != 2 or dashboard.get('handle') != HANDLE:
        raise ValueError('Expected qwq schemaVersion 2 for Lucius7')
    atomic_json(STATE / 'dashboard.json', dashboard)
    errors = read_json(STATE / 'metadata-errors.json', {})
    if args.refresh_history:
        for platform in ('atcoder', 'codeforces'):
            try:
                atomic_json(STATE / f'{platform}-history.json', metadata_history(client, platform))
                errors.pop(platform, None)
            except (RuntimeError, ValueError, OSError) as error:
                errors[platform] = str(error)
                print(f'{platform}: metadata not refreshed: {error}', flush=True)
    atomic_json(STATE / 'metadata-errors.json', errors)
    plan = current_plan()
    print(json.dumps(plan['counts'], ensure_ascii=False))
    print('Plan:', STATE / 'plan.json')


def verify_record(record, entry, candidate):
    if not isinstance(record, dict):
        raise ValueError('Source records must be objects')
    platform = entry['platform']
    if record.get('platform') != platform or str(record.get('id')) != str(candidate['id']):
        raise ValueError('Submission identity mismatch')
    if not isinstance(record.get('handle'), str) or record['handle'].casefold() != HANDLE.casefold():
        raise ValueError('Source is not attributed to Lucius7')
    actual, expected = urlparse(record.get('url', '')), urlparse(candidate['url'])
    if (actual.scheme, actual.hostname, actual.path) != (expected.scheme, expected.hostname, expected.path):
        raise ValueError('Source page differs from the selected submission')
    if actual.username or actual.password or actual.port not in (None, 443) or actual.query or actual.fragment:
        raise ValueError('Untrusted source page URL')
    if record.get('problemId') != entry['problemId']:
        raise ValueError('Source belongs to another problem')
    if record.get('contestId') != entry['contestId']:
        raise ValueError('Source belongs to another contest')
    accepted = {'AC', 'OK', 'ACCEPTED', 'AC ✓'}
    verdict = str(record.get('verdict', '')).strip().upper()
    if not verdict:
        raise ValueError('Missing visible submission verdict')
    if str(candidate.get('verdict', 'AC')).strip().upper() in accepted and verdict not in accepted:
        raise ValueError('Expected an accepted submission')
    language = record.get('language', '')
    extension = language_extension(language)
    if candidate.get('language') and language_extension(candidate['language']) != extension:
        raise ValueError('Source language differs from the selected submission')
    code = record.get('code')
    if not isinstance(code, str) or not code.strip() or '\x00' in code:
        raise ValueError('Empty or invalid source')
    code = code.replace('\r\n', '\n').replace('\r', '\n')
    body = code.encode('utf-8')
    if len(body) > 2_000_000:
        raise ValueError('Source exceeds size limit')
    if 'captureFormat' in record and record['captureFormat'] not in {'plaintext_pre', 'rendered_lines'}:
        raise ValueError('Unknown browser source capture format')
    if 'sourceLines' in record:
        lines = record['sourceLines']
        if type(lines) is not int or lines <= 0 or lines != code.count('\n') + 1:
            raise ValueError('Source line count differs from the captured block')
    for label, size in (('page', record.get('codeSize')), ('history', candidate.get('codeSize'))):
        if size is None:
            continue
        if isinstance(size, str) and re.fullmatch(r'[0-9]+', size):
            size = int(size)
        if type(size) is not int or not 0 < size <= 2_000_000:
            raise ValueError(f'Invalid {label} source size')
        if size not in {len(body), len(body) + code.count('\n')}:
            raise ValueError(f'Source size mismatch: received {len(body)}, {label} reports {size}')
    return extension, body


@locked
def import_sources(records):
    if not isinstance(records, list) or not 1 <= len(records) <= 50:
        raise ValueError('Expected 1–50 browser source records')
    if any(not isinstance(record, dict) or not isinstance(record.get('key'), str) for record in records):
        raise ValueError('Every source record needs a plan key')
    if len({record['key'] for record in records}) != len(records):
        raise ValueError('Duplicate plan keys in one source batch')
    plan = current_plan()
    entries = {entry['key']: entry for entry in plan['entries']}
    results = []
    for record in records:
        entry = entries.get(record.get('key'))
        try:
            if entry is None:
                raise ValueError('Unknown plan entry')
            if entry['status'] == 'existing':
                results.append({'key': entry['key'], 'status': 'existing'})
                continue
            if entry['status'] != 'missing' or not entry['candidates']:
                raise ValueError('Entry is not ready to import')
            # Only the chosen latest AC (or latest attempt when no AC) is accepted.
            candidate = entry['candidates'][0]
            extension, body = verify_record(record, entry, candidate)
            target = safe_file(entry['targetStem'] + extension)
            receipt = {key: record[key] for key in ('platform', 'id', 'problemId', 'contestId', 'handle', 'language', 'verdict', 'url')}
            receipt.update(path=target.relative_to(ROOT).as_posix(), importedAt=now(),
                           submissionEpoch=candidate.get('epoch'), sha256=hashlib.sha256(body).hexdigest(),
                           bytes=len(body), dashboardGeneratedAt=plan['dashboardGeneratedAt'],
                           collectionMethod='normal_browser', lineEndings='LF')
            for field in ('captureFormat', 'sourceLines'):
                if field in record:
                    receipt[field] = record[field]
            publish_source(body, receipt)
            results.append({'key': entry['key'], 'status': 'imported', 'path': receipt['path']})
        except (ValueError, OSError, KeyError, TypeError) as error:
            results.append({'key': record.get('key'), 'status': 'error', 'reason': str(error)})
            if (STATE / 'pending-import.json').exists():
                break  # Preserve the journal; the next operation recovers it first.
    return results


@locked
def import_qoj_history(envelope):
    if not isinstance(envelope, dict) or set(envelope) != {'records', 'pages', 'complete', 'expectedCount'}:
        raise ValueError('QOJ history needs records, pages, complete and expectedCount')
    records, pages, count = envelope['records'], envelope['pages'], envelope['expectedCount']
    if envelope['complete'] is not True or type(count) is not int or not 0 < count <= 100_000:
        raise ValueError('QOJ history must be a complete, counted export')
    if not isinstance(records, list) or len(records) != count:
        raise ValueError('QOJ record count differs from expectedCount')
    if (not isinstance(pages, list) or any(type(page) is not int for page in pages)
            or pages != list(range(1, (count + 9) // 10 + 1))):
        raise ValueError('QOJ export must cover every consecutive page of ten submissions')
    identifiers = set()
    for record in records:
        if (not isinstance(record, dict) or record.get('platform') != 'qoj'
                or record.get('handle') != HANDLE):
            raise ValueError('Unexpected history owner/platform')
        sid = record.get('id')
        if (isinstance(sid, bool) or not isinstance(sid, (int, str))
                or not re.fullmatch(r'[1-9][0-9]*', str(sid)) or str(sid) in identifiers):
            raise ValueError('QOJ submission IDs must be positive and unique')
        identifiers.add(str(sid))
    dashboard = read_json(STATE / 'dashboard.json')
    if not isinstance(dashboard, dict) or dashboard.get('handle') != HANDLE:
        raise ValueError('Run the plan command first')
    known_count = dashboard.get('sources', {}).get('qoj', {}).get('submissionCount')
    if known_count is not None and (type(known_count) is not int or count < known_count):
        raise ValueError('QOJ export has fewer records than the dashboard snapshot')
    previous = histories()
    if count < sum(row.get('platform') == 'qoj' for row in previous):
        raise ValueError('QOJ export would discard previously captured history')
    other_platforms = [row for row in previous if row.get('platform') != 'qoj']
    # Full replacement: old QOJ verdicts/metadata must not conflict with new rows.
    history = [*other_platforms, *records]
    catalog = read_json(ROOT / 'archive' / 'contest-mappings.json')
    build_plan(apply_contest_mappings(dashboard, history, catalog), ROOT, history)
    atomic_json(STATE / 'qoj-history.json', envelope)
    return {'qojHistory': count, 'counts': current_plan()['counts']}


def intake_handler(token):
    class Handler(BaseHTTPRequestHandler):
        def log_message(self, fmt, *values):
            pass  # Do not log request bodies, source or auth data.

        def respond(self, status, body):
            self.send_response(status)
            self.send_header('Content-Type', 'text/html; charset=utf-8')
            self.send_header('Cache-Control', 'no-store')
            self.send_header('Content-Security-Policy', "default-src 'none'; style-src 'unsafe-inline'; form-action 'self'; frame-ancestors 'none'")
            self.end_headers()
            self.wfile.write(body.encode('utf-8'))

        def do_GET(self):
            import html
            if self.headers.get('Host') != f'127.0.0.1:{self.server.server_port}':
                self.respond(403, 'Unexpected intake host')
                return
            if self.path != '/':
                self.respond(404, 'Not found')
                return
            plan = current_plan()
            # Keep the browser form responsive while the complete plan stays on disk.
            queue = [entry for entry in plan['entries'] if entry['status'] in ('missing', 'needs_submission', 'ambiguous')][:50]
            state = json.dumps({'counts': plan['counts'], 'entries': queue}, ensure_ascii=False)
            self.respond(200, '<!doctype html><meta charset="utf-8"><title>Contest archive intake</title>'
                         '<style>body{font:16px system-ui;max-width:1100px;margin:24px auto}textarea{width:100%;height:200px}pre{white-space:pre-wrap}</style>'
                         '<h1>本地源码归档</h1><p>仅保存已验证的本人提交，不覆盖已有文件。</p>'
                         '<form method="post"><input type="hidden" name="token" value="' + token + '">'
                         '<label>类型 <select name="kind"><option value="sources">源码</option><option value="qoj-history">QOJ 提交历史</option></select></label>'
                         '<p><label for="payload">JSON 数据</label><textarea id="payload" name="payload"></textarea></p>'
                         '<button type="submit">校验并归档</button></form><h2>待处理清单（前 50 项）</h2><pre id="queue">' + html.escape(state) + '</pre>')

        def do_POST(self):
            import html
            try:
                origin = f'http://127.0.0.1:{self.server.server_port}'
                if self.path != '/' or self.headers.get('Origin') != origin or self.headers.get('Host') != origin.split('//')[1]:
                    raise ValueError('Unexpected intake origin')
                size = int(self.headers.get('Content-Length', 0))
                if not 0 < size <= 8_000_000:
                    raise ValueError('Invalid intake size')
                if (self.headers.get('Transfer-Encoding')
                        or self.headers.get('Content-Type', '').split(';')[0].strip().lower()
                        != 'application/x-www-form-urlencoded'):
                    raise ValueError('Expected a bounded URL-encoded form')
                body = self.rfile.read(size)
                if len(body) != size:
                    raise ValueError('Incomplete intake request')
                values = parse_qs(body.decode('utf-8'), keep_blank_values=True,
                                  strict_parsing=True, max_num_fields=3)
                if set(values) != {'token', 'kind', 'payload'} or any(len(v) != 1 for v in values.values()):
                    raise ValueError('Unexpected or duplicate intake fields')
                if not secrets.compare_digest(values['token'][0], token):
                    raise ValueError('Invalid intake token')
                if values['kind'][0] not in {'qoj-history', 'sources'}:
                    raise ValueError('Unknown intake kind')
                records = json.loads(values['payload'][0])
                if values.get('kind') == ['qoj-history']:
                    result = import_qoj_history(records)
                else:
                    result = import_sources(records)
                self.respond(200, '<meta charset="utf-8"><title>Archive import result</title><pre id="result">' + html.escape(json.dumps(result, ensure_ascii=False, indent=2)) + '</pre><a href="/">继续归档</a>')
            except (ValueError, OSError, KeyError, TypeError) as error:
                self.respond(400, '<meta charset="utf-8"><pre>' + html.escape(str(error)) + '</pre><a href="/">返回</a>')
    return Handler


def serve(args):
    server = HTTPServer(('127.0.0.1', args.port), intake_handler(secrets.token_urlsafe(32)))
    print(f'Archive intake: http://127.0.0.1:{server.server_port}/', flush=True)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest='command', required=True)
    prepare_parser = commands.add_parser('plan')
    prepare_parser.add_argument('--dashboard', help='Path to an OJFlare schemaVersion 2 snapshot (default: public API)')
    prepare_parser.add_argument('--refresh-history', action='store_true')
    server_parser = commands.add_parser('serve')
    server_parser.add_argument('--port', type=int, default=4181)
    commands.add_parser('status')
    args = parser.parse_args()
    if args.command == 'plan':
        prepare(args)
    elif args.command == 'serve':
        serve(args)
    else:
        print(json.dumps(current_plan()['counts'], ensure_ascii=False, indent=2))


if __name__ == '__main__':
    main()
