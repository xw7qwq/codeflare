#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    std::vector<int> x(3);
    for (int i = 0; i < 3; i++) {
        std::cin >> x[i];
    }
    
    std::sort(x.begin(), x.end());

    std::cout << x[2] - x[0] << "\n";
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
