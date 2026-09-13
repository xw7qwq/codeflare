#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    std::cin >> n;

    std::vector<int> v;
    for (int i = 1; i <= n; i++) {
        auto s = std::to_string(i);
        if (std::count(s.begin(), s.end(), '4') + std::count(s.begin(), s.end(), '7') == s.size()) {
            v.push_back(i);
        }
    }

    for (auto x : v) {
        if (n % x == 0) {
            std::cout << "YES\n";
            return;
        }
    }
    std::cout << "NO\n";
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
