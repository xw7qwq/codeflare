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

    std::vector<int> p;
    for (int i = 0; i < n; i++) {
        if (a[i] != i + 1) {
            p.push_back(a[i]);
        }
    }

    std::reverse(p.begin(), p.end());

    for (int i = n - 1; i >= 0; i--) {
        if (a[i] != i + 1) {
            if (p.back() == i + 1) {
                p.pop_back();
            } else {
                std::cout << "NO\n";
                return;
            }
        }
    }
    std::cout << "YES\n";
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
