#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    std::cin >> n;

    std::vector<std::string> v(n);
    for (int i = 0; i < n; i++) {
        std::cin >> v[i];
    }

    int ans = 0;
    for (int l = 0, r = 0; l < n; l = r) {
        while (r < n && v[r] == v[l]) {
            r++;
        }
        ans++;
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
