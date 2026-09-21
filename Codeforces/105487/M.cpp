#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N;
    std::cin >> N;

    std::vector<std::vector<int>> adj(N);
    for (int i = 1; i < N; i++) {
        int fa;
        std::cin >> fa;
        fa--;
        adj[fa].push_back(i);
        adj[i].push_back(fa);
    }

    auto check = [&](int m) -> bool {
        std::vector<int> d(N);
        auto dfs = [&](this auto &&self, int x, int fa) {
            if (x != 0 && adj[x].size() == 1) {
                d[x] = m;
                return;
            }

            for (auto y : adj[x]) {
                if (y == fa) {
                    continue;
                }
                self(y, x);
                d[x] = std::max(d[x], d[y] - 1);
            }
        };

        dfs(0, -1);

        for (int i = 1; i < N; i++) {
            if (d[i] == 0) {
                return false;
            }
        }
        return true;

    };


    int lo = 1, hi = N;
    while (lo < hi) {
        int m = (lo + hi) >> 1;
        if (check(m)) {
            hi = m;
        } else {
            lo = m + 1;
        }
    }
    std::cout << lo << "\n";
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
