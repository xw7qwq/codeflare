#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int z0, h, u0, v0, u1, v1;
    std::cin >> z0 >> h >> u0 >> v0 >> u1 >> v1;
    h += z0;

    int q;
    std::cin >> q;

    int xmn = std::min(u0, u1), xmx = std::max(u0, u1);
    int ymn = std::min(v0, v1), ymx = std::max(v0, v1);
    int zmn = std::min(z0, h), zmx = std::max(z0, h);

    while (q--) {
        int x, y, z;
        std::cin >> x >> y >> z;

        if (x >= xmn && x <= xmx && y >= ymn && y <= ymx && z >= zmn && z <= zmx) {
            std::cout << "YES\n";
        } else {
            std::cout << "NO\n";
        }
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
