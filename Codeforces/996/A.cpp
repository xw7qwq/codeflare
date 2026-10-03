#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    std::vector<int> a{100, 20, 10, 5, 1};
    int x;
    std::cin >> x;

    int cnt = 0;
    for (auto y : a) {
        cnt += x / y;
        x %= y;
    }
    std::cout << cnt;
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
