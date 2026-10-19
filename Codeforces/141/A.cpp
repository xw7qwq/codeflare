#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    std::string s, t;
    std::cin >> s >> t;
    s += t;

    std::string u;
    std::cin >> u;

    std::sort(s.begin(), s.end());
    std::sort(u.begin(), u.end());

    std::cout << (s == u ? "YES" : "NO") << "\n";
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
