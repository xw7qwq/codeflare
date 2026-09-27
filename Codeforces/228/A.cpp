#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    std::set<int> s;
    for (int i = 0; i < 4; i++) {
        int x;
        std::cin >> x;
        s.insert(x);
    }
    std::cout << 4 - s.size() << "\n";
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
