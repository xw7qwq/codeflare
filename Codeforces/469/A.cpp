#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    std::cin >> n;

    std::vector<bool> st(n);
    for (int _ = 0; _ < 2; _++) {
        int p;
        std::cin >> p;
        for (int i = 0; i < p; i++) {
            int a;
            std::cin >> a;
            a--;
            if (!st[a]) {
                n--;
                st[a] = true;
            }
        }
    }

    std::cout << (n ? "Oh, my keyboard!" : "I become the guy.") << "\n";
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
