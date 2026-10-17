#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    std::cin >> n;

    std::vector<std::array<int, 4>> a(n);
    for (int i = 0; i < n; i++) {
        std::cin >> a[i][1];
    }
    for (int i = 0; i < n; i++) {
        std::cin >> a[i][2];
    }
    for (int i = 0; i < n; i++) {
        if (a[i][1] < a[i][2]) {
            a[i][0] = -1;
        } else if (a[i][1] == a[i][2]) {
            a[i][0] = 0;
        } else {
            a[i][0] = 1;
        }
        a[i][3] = i;
    }

    std::sort(a.begin(), a.end(), [&](auto x, auto y) {
        if (x[0] != y[0]) {
            return x[0] < y[0];
        } 

        if (x[0] == -1) {
            if (x[1] != y[1]) {
                return x[1] < y[1];
            }
        } else {
            if (x[2] != y[2]) {
                return x[2] > y[2];
            }
        }
        return x[3] < y[3];
    });

    int ta = 0, tb = 0;
    for (int i = 0; i < n; i++) {
        ta += a[i][1];
        tb = std::max(tb, ta);
        tb += a[i][2];
    }

    std::cout << tb << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << a[i][3] + 1 << " \n"[i == n - 1];
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
