#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n, m;
    std::cin >> n >> m;

    for (int i = 0; i < n; i++) {
        if (i % 2 == 0) {
            std::cout << std::string(m, '#') << "\n";
        } else {
            if ((i - 1) / 2 % 2 == 0) {
                std::cout << std::string(m - 1, '.') + "#\n"; 
            } else {
                std::cout << "#" + std::string(m - 1, '.') << "\n";
            }
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
