#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    std::string s;
    std::cin >> s;

    
    for (int i = 1; i < s.size(); i++) {
        if (std::islower(s[i])) {
            std::cout << s << "\n";
            return;
        }
    }

    for (int i = 0; i < s.size(); i++) {
        if (std::islower(s[i])) {
            s[i] = std::toupper(s[i]);
        } else {
            s[i] = std::tolower(s[i]);
        }
        std::cout << s[i];
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
