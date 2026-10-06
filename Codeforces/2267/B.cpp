#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    std::cin >> n;

    std::vector<int> cnt(101);
    for (int i = 0; i < n; i++) {
        int a;
        std::cin >> a;
        cnt[a]++;
    }

    int x = 100;
    while (n) {
        if (cnt[x]) {
            cnt[x]--;
            std::cout << x << " ";
            n--;
        }
        x = (x - 2 + 100) % 100 + 1;
    }
    std::cout << "\n";
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
