#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n, m;
    std::cin >> n >> m;

    std::vector<int> w(n);
    for (int i = 0; i < n; i++) {
        std::cin >> w[i];
    }

    std::vector g(n, std::vector<int>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            std::cin >> g[i][j];
        }
    }

    int ans = m;
    std::multiset<int> ms;
    for (int i = n - 1; i >= 0; i--) {
        for (int j = 0; j < m; j++) {
            ms.insert(g[i][j]);
        }
        
        int sum = 0, cur = 0;
        for (auto it = ms.rbegin(); it != ms.rend(); it++) {
            if (cur + 1 >= ans) {
                break;
            }
            sum += *it;
            if (sum >= w[i]) {
                ans = std::min(ans, cur + 1);
            }
            cur++;
        }
    }
    std::cout << ans << "\n";
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
