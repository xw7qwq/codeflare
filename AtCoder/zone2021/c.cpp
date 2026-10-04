#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N;
    std::cin >> N;

    std::vector<std::array<int, 5>> v(N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < 5; j++) {
            std::cin >> v[i][j];
        }
    }

    auto check = [&](int m) {
        std::vector<int> vis(1 << 5);
        for (auto u : v) {
            int x = 0;
            for (int i = 0; i < 5; i++) {
                if (u[i] >= m) {
                    x |= (1 << (i));
                }
            }
            vis[x] = true;
        }

        for (int x = 0; x < (1 << 5); x++) {
            for (int y = 0; y < (1 << 5); y++) {
                for (int z = 0; z < (1 << 5); z++) {
                    if (vis[x] && vis[y] && vis[z] && (x | y | z) == (1 << 5) - 1) {
                        return true;
                    }
                }
            }
        }
        return false;
    };

    int lo = 0, hi = 1E9;
    while (lo < hi) {
        int m = (lo + hi + 1) >> 1;
        if (check(m)) {
            lo = m;
        } else {
            hi = m - 1;
        }
    }
    std::cout << lo << "\n";
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
