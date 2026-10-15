#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    std::cin >> n;

    std::vector<int> d(n + 1);
    for (int k = 1; k <= n; k++) {
        int a;
        std::cin >> a;
        d[std::min(n, a * k)]++;
        d[std::min(n, (a + 1) * k)]--;
    }

    std::vector<int> ans;
    for (int i = 0; i < n; i++) {
        d[i + 1] += d[i];
        if (d[i] == 0) {
            ans.push_back(i);   
        }
    }

    std::cout << ans.size() << "\n";
    for (int i = 0; i < ans.size(); i++) {
        std::cout << ans[i] << " \n"[i == ans.size() - 1];
    }
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
