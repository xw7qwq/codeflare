#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n, m;
    std::cin >> n >> m;

    std::vector<int> a(n);
    std::vector<std::vector<int>> mb(3);
    for (int i = 0; i < n; i++) {
        std::cin >> a[i];
    }
    for (int i = 0; i < n; i++) {
        int b;
        std::cin >> b;
        mb[b].push_back(a[i]);
    }

    std::sort(mb[1].begin(), mb[1].end(), std::greater<>());
    std::sort(mb[2].begin(), mb[2].end(), std::greater<>());

    int sz1 = mb[1].size(), sz2 = mb[2].size();
    std::vector<int> pre1(sz1 + 1), pre2(sz2 + 1);
    for (int i = 0; i < sz1; i++) {
        pre1[i + 1] = pre1[i] + mb[1][i];
    }
    for (int i = 0; i < sz2; i++) {
        pre2[i + 1] = pre2[i] + mb[2][i];
    }

    int ans = 1E18;
    for (int l = -1, r = sz2 - 1; l < sz1; l++) {
        while (r >= 0 && pre1[l + 1] + pre2[r] >= m) {
            r--;
        }
        
        if (pre1[l + 1] + pre2[r + 1] >= m) {
            ans = std::min(ans, (l + 1) * 1 + (r + 1) * 2);
        }
    }
    std::cout << (ans == 1E18 ? -1 : ans) << "\n";
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
