#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n, k;
    std::cin >> n >> k;

    int odds = (n + 1) / 2;
    if (k <= odds) {
        std::cout << 2 * k - 1 << "\n";
    } else {
        k -= odds;
        std::cout << 2 * k << "\n";
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
