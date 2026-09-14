#include <bits/stdc++.h>

#define int long long

using namespace std;
using i64 = long long;

void solve() {
    int n, k, S, T;
    std::cin >> n >> k >> S >> T;
    S--, T--;

    std::vector<int> x(n);
    for (int i = 0; i < n ;i++) {
        std::cin >> x[i];
    }

    int cur = 0;
    std::map<std::pair<int, int>, int> ump;
    std::map<int, std::pair<int, int>> fump;
    std::vector<int> cst;
    auto gt = [&](int x, int y) {
        // x matou; chuanbo y
        if (ump.count({x, y})) {
            return ump[{x, y}];
        } else {
            cst.push_back(0);
            fump[cur] = {x, y};
            ump[{x, y}] = cur;
            return cur++;
        }
    };

    auto fgt = [&](int x) {
        return fump[x];
    };

    std::vector<std::vector<int>> hasc(n);
    std::map<int, std::vector<int>> cis;

   
    for (int i = 0; i < n; i++) {
        int u;
        std::cin >> u;

        std::vector<std::pair<int, int>> p(u);

        for (int j = 0; j < u; j++) {
            int c, w;
            std::cin >> c >> w;
            c--;
            cst[gt(i, c)] = w;
            cis[c].push_back(i);
            hasc[i].push_back(c);
        }
    }

    std::vector<std::vector<std::pair<int, int>>> adj(cur);
    for (auto &[k, v] : cis) {
        std::vector<std::pair<int, int>> nv(v.size());
        for (int i = 0; i < v.size(); i++) {
            nv[i] = {x[v[i]], v[i]};
        }  
        std::sort(nv.begin(), nv.end());


        for (int i = 1; i < nv.size(); i++) {
            auto [dx, x] = nv[i];
            auto [dy, y] = nv[i - 1];
            adj[gt(x, k)].push_back({gt(y, k), std::abs(dx - dy)});
            adj[gt(y, k)].push_back({gt(x, k), std::abs(dx - dy)});
        }
    }

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
    std::vector<int> d(cur, 1E18);

    std::vector<bool> updx(n);

    updx[S] = 1;
    for (auto x : hasc[S]) {
        pq.push({cst[gt(S, x)], gt(S, x)});
        d[gt(S, x)] = cst[gt(S, x)];
    }

    while (!pq.empty()) {
        auto [w, id] = pq.top();
        pq.pop();

        if (d[id] < w) {
            continue;
        }

        auto [x, c] = fgt(id);

        if (x == T) {
            std::cout << w << "\n";
            return;
        }

        if (!updx[x]) {
            updx[x] = 1;
            for (auto xc : hasc[x]) {
                if (xc != c && d[gt(x, xc)] > w + cst[gt(x, xc)]) {
                    pq.push({w + cst[gt(x, xc)], gt(x, xc)});
                    d[gt(x, xc)] = w + cst[gt(x, xc)];
                }
            }
        }

        for (auto [nid, nw] : adj[id]) {
            if (d[nid] > d[id] + nw) {
                d[nid] = d[id] + nw;
                pq.push({d[nid], nid});
            }
        }
    }

    std::cout << -1 << "\n";


}

signed main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int t = 1;
    std::cin >> t;

    while (t--) {
        solve();
    }

    return 0;

}