#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n, q;
    std::cin >> n >> q;

    std::vector<int> d(n + 1);
    std::map<int, std::vector<std::pair<int, int>>> mp;
    while (q--) {
        int l, r, x;
        std::cin >> l >> r >> x;
        l--, r--;
        mp[x].push_back({l, r});
    }

    for (auto &[_, v] : mp) {
        std::sort(v.begin(), v.end());
        std::vector<std::pair<int, int>> nv;
        for (auto [l, r] : v) {
            if (!nv.empty() && l <= nv.back().second) {
                nv.back().second = std::max(nv.back().second, r);
            } else {
                nv.push_back({l, r});
            }
        }

        for (auto [l, r] : nv) {
            d[l]++;
            d[r + 1]--;
        }
    }

    for (int i = 0; i < n; i++) {
        d[i + 1] += d[i];
        std::cout << d[i] << " \n"[i == n - 1];
    }
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
