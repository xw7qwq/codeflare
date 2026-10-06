#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int A, B;
    std::cin >> A >> B;

    for (int d = B - A; d >= 1; d--) {
        if (B / d - (A - 1) / d >= 2) {
            std::cout << d << "\n";
            return;
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
