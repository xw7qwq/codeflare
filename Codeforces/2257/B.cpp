#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n, m;
    std::cin >> n >> m;

    std::vector<int> a(n), b(m);
    for (int i = 0; i < n; i++) {
        std::cin >> a[i];
    }
    for (int i = 0; i < m; i++) {
        std::cin >> b[i];
    }

    int sa = a.back(), sb = b.back();
    for (int i = 0; i < n - 1; i++) {
        sa += a[i] - a[i + 1] + 1;
    }
    for (int i = 0; i < m - 1; i++) {
        sb += b[i] - b[i + 1] + 1;
    }

    std::cout << (sa >= sb ? 1 : 2) << "\n";

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
