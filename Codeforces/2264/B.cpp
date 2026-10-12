#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n, m;
    std::cin >> n >> m;

    std::vector<int> a(n);
    for (int i = 0; i < n; i++) {
        std::cin >> a[i];
    }

    int ans = -1E18;

    int sum = 0;
    std::priority_queue<int> pq;
    for (int i = 0; i < m - 1; i++) {
        pq.push(a[i]);
        sum += a[i];
    }

    for (int i = m - 1; i < n; i++) {
        ans = std::max(ans, m * a[i] - sum);
        pq.push(a[i]);
        sum += a[i];
        auto t = pq.top();
        sum -= t;
        pq.pop();
    }
    std::cout << ans << "\n";
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
