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

    std::vector<int> c(n);
    for (int i = 0; i < n; i++) {
        std::cin >> c[i];
    }

    DSU dsu(n);
    std::vector<std::pair<int, int>> es;
    for (int i = 0; i < n - 1; i++) {
        int x, y;
        std::cin >> x >> y;
        x--, y--;
        if (c[x] != c[y]) {
            es.push_back({x, y});
        } else {
            dsu.merge(x, y);
        }
    }

    std::vector<std::vector<int>> adj(n);
    for (auto [x, y] : es) {
        adj[dsu.find(x)].push_back(dsu.find(y));
        adj[dsu.find(y)].push_back(dsu.find(x));
    }

    std::vector<bool> vis(n);
    int ans = 0;
    for (int i = 0; i < n; i++) {
        int x = dsu.find(i);
        if (!vis[x]) {
            std::map<int, std::vector<int>> has;
            for (auto y : adj[x]) {
                has[c[y]].push_back(dsu.size(y));
            }
            for (auto &[_, v] : has) {
                int s = 0;
                for (auto w : v) {
                    ans += s * w;
                    s += w;
                }
            }
            vis[x] = true;
        }
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
