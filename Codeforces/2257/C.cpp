#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    std::cin >> n;

    std::vector<int> p(n);
    for (int i = 1; i < n; i++) {
        std::cin >> p[i];
        p[i]--;
    }

    int m;
    std::cin >> m;
    std::vector<int> a(m);
    std::vector<bool> vis(n);
    for (int i = 0; i < m; i++) {
        std::cin >> a[i];
        a[i]--;
        vis[a[i]] = true;
    }

    std::cout << m - 1 << " ";
    if (vis[0]) {
        for (int i = 0; i < m; i++) {
            if (a[i] != 0) {
                std::cout << a[i] + 1 << " ";
            }
        }
        std::cout << "\n";
    } else {
        bool ok = false;
        for (int i = 0; i < m; i++) {
            if (!vis[p[a[i]]] && !ok) {
                ok = true;
                continue;
            }
            std::cout << a[i] + 1 << " ";
        }
        std::cout << "\n";
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
