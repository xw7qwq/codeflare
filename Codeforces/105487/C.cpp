#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    std::string s;
    std::cin >> s;

    int x = std::count(s.begin(), s.end(), 'P'), y = std::count(s.begin(), s.end(), 'C');
    // if (y < 5) {
    //     if (x) {
    //         std::cout << 1 << "\n";
    //     } else {
    //         std::cout << 0 << "\n";
    //     }
    //     return;
    // }

    if ((2 * x + 1) < y) {
        std::cout << x << "\n";
    } else {
        std::cout << (y - 1) / 2 << "\n";
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
