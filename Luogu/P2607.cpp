#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

struct DSU {
    std::vector<int> f, siz;
    
    DSU() {}
    DSU(int n) {
        init(n);
    }
    
    void init(int n) {
        f.resize(n);
        std::iota(f.begin(), f.end(), 0);
        siz.assign(n, 1);
    }
    
    int find(int x) {
        while (x != f[x]) {
            x = f[x] = f[f[x]];
        }
        return x;
    }
    
    bool same(int x, int y) {
        return find(x) == find(y);
    }
    
    bool merge(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) {
            return false;
        }
        siz[x] += siz[y];
        f[y] = x;
        return true;
    }
    
    int size(int x) {
        return siz[find(x)];
    }
};

void solve() {
    int n;
    std::cin >> n;

    std::vector<int> a(n);

    std::vector<std::vector<int>> adj(n);
    DSU dsu(n);
    std::vector<std::pair<int, int>> e;
    for (int x = 0; x < n; x++) {
        std::cin >> a[x];
        int y;
        std::cin >> y;
        y--;
        if (dsu.merge(x, y)) {
            adj[x].push_back(y);
            adj[y].push_back(x);
        } else {
            e.push_back({x, y});
        }
    }

    std::vector<std::array<int, 2>> dp(n);

    constexpr int inf = 1E18;
    int tt = -1;
    auto dfs = [&](this auto &&self, int x, int fa) -> void {
        dp[x][0] = 0;
        if (x == tt) {
            dp[x][1] = -inf;
        } else {
            dp[x][1] = a[x];
        }

        for (auto y : adj[x]) {
            if (y == fa) {
                continue;
            }
            self(y, x);
            dp[x][0] += std::max(dp[y][0], dp[y][1]);
            dp[x][1] += dp[y][0];
        }
    };

    int ans = 0;
    for (auto [x, y] : e) {
        tt = x;
        dfs(x, -1);
        int res = std::max(dp[x][1], dp[x][0]);
        tt = y;
        dfs(x, -1);
        res = std::max({res, dp[x][1], dp[x][0]});
        ans += res;
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
