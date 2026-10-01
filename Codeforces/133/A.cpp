#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    std::string s;
    std::cin >> s;

    auto t = std::string::npos;
    if (s.find('H') == t && s.find('Q') == t && s.find('9') == t) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
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
