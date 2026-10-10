#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int a, b, c;
    std::cin >> a >> b >> c;

    std::cout << std::max({a + b + c, a * (b + c), a * b * c, (a + b) * c, a * b + c, a + b * c}) << "\n";
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
