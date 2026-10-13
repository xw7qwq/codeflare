#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n, m;
    std::cin >> n >> m;

    std::vector<int> vis(26);
    for (int i = 0; i < n; i++) {
        std::string s;
        std::cin >> s;
        vis[s[0] - 'a'] = true;
    }

    bool ok = true;
    for (int i = 0; i < m; i++) {
        std::string s;
        std::cin >> s;
        for (auto c : s) {
            if (!vis[c - 'A']) {
                ok = false;
            }
        }
    }
    std::cout << (ok ? "YES" : "NO") << "\n";
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
