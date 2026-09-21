#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    std::string s;
    std::cin >> s;

    std::vector<int> a;
    for (int l = 0, r = 0; l < s.size(); l = r) {
        int cnt = 0;
        while (r < s.size() && s[l] == s[r]) {
            r++;
        }
        if (s[l] == '1') {
            a.push_back(r - l);
        }
    }

    long double ans = 0;
    for (auto x : a) {
        if (x % 2 == 1) {
            ans += x / 2 + 1;
        } else {
            ans += (x - 2) / 2 + std::sqrt(2.L);
        }
    }
    std::cout << std::fixed << std::setprecision(12) << ans << "\n";
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
