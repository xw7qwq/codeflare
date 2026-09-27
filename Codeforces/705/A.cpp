#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    std::cin >> n;

    for (int i = 1; i <= n; i++) {
        if (i == 1) {
            std::cout << "I hate ";
        } else {
            std::cout << (i % 2 ? "that I hate " : "that I love ");
        }

        if (i == n) {
            std::cout << "it" << "\n";
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
