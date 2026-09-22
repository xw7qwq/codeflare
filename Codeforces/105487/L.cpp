#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int A, B, C, D;
    std::cin >> A >> B >> C >> D;

    if (A < 4) {
        std::cout << 0 << "\n";
        return;
    }


    int ans = 0;
    int E = std::min(B, C);
    for (int x = 0; x * x <= D; x++) {
        int y = E - x;
        if (x > 0) {
            y = std::min(y, D / x);
        }
        ans = std::max(ans, (x + 2) * (y + 2));
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
