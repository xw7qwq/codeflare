#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    std::string s = "BYR";
    char c;
    std::cin >> c;

    for (int i = 0; i < s.size(); i++) {
        if (c == s[i]) {
            std::cout << s[(i + 1) % s.size()] << "\n";
            return;
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
