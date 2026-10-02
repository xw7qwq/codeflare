#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int Q;
    std::cin >> Q;

    std::string S, T;
    std::cin >> S >> T;

    int n = S.size(), m = T.size();
    std::vector<std::pair<int, int>> v;
    for (int i = 0; i + m - 1 < n; i++) {
        if (S.substr(i, m) == T) {
            v.push_back({i, i + m - 1});
        }
    }

    while (Q--) {
        int l, r;
        std::cin >> l >> r;
        l--, r--;
        auto u = std::lower_bound(v.begin(), v.end(), std::pair{l, -1});
        if (u != v.end() && u -> second <= r) {
            std::cout << "Yes\n";
        } else {
            std::cout << "No\n";
        }
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
