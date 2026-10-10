#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    std::vector<int> a(4);
    for (int i = 0; i < 4; i++) {
        std::cin >> a[i];
    }
    int d;
    std::cin >> d;

    int cnt = 0;
    for (int x = 1; x <= d; x++) {
        for (int i = 0; i < 4; i++) {
            if (x % a[i] == 0) {
                cnt++;
                break;
            }
        }
    }
    std::cout << cnt << "\n";
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
