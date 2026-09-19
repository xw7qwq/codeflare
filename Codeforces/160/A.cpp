#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    std::cin >> n;

    std::vector<int> a(n);
    for (int i = 0; i < n; i++) {
        std::cin >> a[i];
    }

    std::sort(a.begin(), a.end(), std::greater<>());
    int s = std::accumulate(a.begin(), a.end(), 0LL);

    int pre = 0;
    for (int i = 0; i < n; i++) {
        pre += a[i];
        if (pre > s - pre) {
            std::cout << i + 1 << "\n";
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
