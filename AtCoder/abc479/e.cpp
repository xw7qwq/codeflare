#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N;
    std::cin >> N;

    std::vector<std::vector<std::pair<int, int>>> adj(N);
    for (int i = 0; i < N - 1; i++) {
        int x, y;
        std::cin >> x >> y;
        x--, y--;
        adj[x].push_back({y, i + 1});
        adj[y].push_back({x, i + 1});
    }

    std::vector<int> sz(N);
    int cen = 0;
    auto dfs1 = [&](this auto &&self, int x, int fa) -> void {
        sz[x] = 1;
        int mx = 0;
        for (auto [y, w] : adj[x]) {
            if (y == fa) {
                continue;
            }
            self(y, x);
            sz[x] += sz[y];
            mx = std::max(mx, sz[y]);
        }
        mx = std::max(mx, N - sz[x]);
        if (mx <= N / 2) {
            cen = x;
        }
    };

    dfs1(0, -1);

    std::vector<int> ans(N, N * (N - 1) / 2);
    auto dfs2 = [&](this auto &&self, int x, int fa, int c) -> void {
        ans[x] -= c;
        for (auto [y, w] : adj[x]) {
            if (y == fa) {
                continue;
            }
            self(y, x, c + w);
        }
    };
    dfs2(cen, -1, 0);

    for (int i = 0; i < N; i++) {
        std::cout << ans[i] << " \n"[i == N - 1];
    }
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
