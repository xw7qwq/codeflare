#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    std::string s;
    std::cin >> s;

    std::string ans;
    for (int l = 0; l < s.size(); ) {
        if (l + 2 < s.size() && s.substr(l, 3) == "WUB") {
            while (l + 2 < s.size() && s.substr(l, 3) == "WUB") {
                l += 3;
            }
            ans.push_back(' ');
        } else {
            while (l < s.size() && s.substr(l, 3) != "WUB") {
                ans.push_back(s[l]);
                l++;
            }
        }
    }
    std::cout << ans << "\n";
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
