#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n, k;
    std::cin >> n >> k;

    std::vector<int> a(n);
    int sum = 0;
    for (int i = 0; i < n; i++) {
        std::cin >> a[i];
        sum += a[i];
    }

    for (int i = 0; i < k - 1; i++) {
        if (i < n - k + 1) {
            sum -= std::min(a[i], a[n - 1 - i]);
        } else {
            sum -= a[i];
        }
    }
    std::cout << sum << "\n";
}

signed main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int t = 1;
    std::cin >> t;
    while (t--) {
        solve();
    }

    return 0;
}
