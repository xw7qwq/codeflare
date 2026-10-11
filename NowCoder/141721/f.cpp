#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    std::cin >> n;

    std::vector<std::pair<int, int>> p(n);
    for (int i = 0; i < n; i++) {
        std::cin >> p[i].first >> p[i].second;
    }

    auto check = [&](int m) {
        auto [l, r] = p.front();
        int R = -1;
        std::vector<int> l1(n), r1(n);
        for (int i = 1; i < n; i++) {
            auto [nl, nr] = p[i];
            nl = std::max(nl, l + 1);
            nr = std::min(nr, r + m);
            if (nl <= nr) {
                R = i;
                l = nl;
                r = nr;
                l1[i] = l;
                r1[i] = r;
            } else {
                break;
            }
        }

        l = p.back().first;
        r = p.back().second;
        int L = n;
        std::vector<int> l2(n), r2(n);
        for (int i = n - 2; i >= 0; i--) {
            auto [nl, nr] = p[i];
            nl = std::max(nl, l + 1);
            nr = std::min(nr, r + m);
            if (nl <= nr) {
                L = i;
                l = nl;
                r = nr;
                l2[i] = l;
                r2[i] = r;
            } else {
                break;
            }
        }

        if (L != n && R != -1) {
            for (int i = L; i <= R; i++) {
                if (l2[i] > r1[i] || l1[i] > r2[i]) {

                } else {
                    return true;
                }
            }
        }
        return false;
    };

    int lo = 1, hi = 2E9;
    if (!check(hi)) {
        std::cout << -1 << "\n";
        return;
    }

    while (lo < hi) {
        int m = (lo + hi) >> 1;
        if (check(m)) {
            hi = m;
        } else {
            lo = m + 1;
        }
    }

    std::cout << lo << "\n";
}

signed main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int t = 1;
    // std::cin >> t;
    while (t--) {
        solve();
    }

    return 0;
}
