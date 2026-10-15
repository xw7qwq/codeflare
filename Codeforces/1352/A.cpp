#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    std::string s;
    std::cin >> s;


    std::vector<std::string> ans;
    for (int i = 0; i < s.size(); i++) {
        if (s[i] != '0') {
            std::string t{s[i]};
            t += std::string(s.size() - i - 1, '0');
            ans.push_back(t);
        }
    }
    std::cout << ans.size() << "\n";
    for (auto x : ans) {
        std::cout << x << " ";
    }
    std::cout << "\n";
}

signed main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int t = 1;
    std::cin >> t;
    while (t--) {
        solve();
    }

    return 0;
}
