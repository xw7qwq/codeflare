#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n, k;
    std::cin >> n >> k;

    if (k >= n && k <= 2 * n - 1) {
        int d = k - n;
        std::vector g(n, std::vector<int>(n));
        std::vector<bool> vis(n * n + 1);
        for (int i = 1; i < n; i++) {
            if (d) {
                g[i][i] = i * n + i;
                d--;
            } else {
                g[i][i] = i + 1;
            }
            vis[g[i][i]] = true;
        }
        int cur = 1;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (g[i][j] == 0) {
                    while (vis[cur]) {
                        cur++;
                    }
                    g[i][j] = cur;
                    vis[cur] = true;
                }
            }
        }

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                std::cout << g[i][j] << " \n"[j == n - 1];
            }
        }

    } else {
        std::cout << -1 << "\n";
    }
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
