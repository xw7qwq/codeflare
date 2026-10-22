#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int s, n;
    std::cin >> s >> n;

    std::vector<std::pair<int, int>> d(n);
    for (int i = 0; i < n; i++) {
        std::cin >> d[i].first >> d[i].second;
    }
    std::sort(d.begin(), d.end());

    for (int i = 0; i < n; i++) {
        auto [x, y] = d[i];
        if (s > x) {
            s += y;
        } else {
            std::cout << "NO\n";
            return;
        }
    }
    std::cout << "YES\n";
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
