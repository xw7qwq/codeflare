#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    std::cin >> n;

    std::vector<int> a(n);
    int mx = -1, mn = 200;
    for (int i = 0; i < n; i++) {
        std::cin >> a[i];
        mx = std::max(mx, a[i]);
        mn = std::min(mn, a[i]);
    }

    int l, r;
    for (int i = 0; i < n; i++) {
        if (a[i] == mx) {
            l = i;
            break;
        }
    }
    for (int i = n - 1; i >= 0; i--) {
        if (a[i] == mn) {
            r = i;
            break;
        }
    }

    bool ok = false;
    if (l > r) {
        ok = true;
    }

    std::cout << l + n - 1 - r - ok << "\n";
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
