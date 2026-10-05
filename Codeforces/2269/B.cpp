#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    std::cin >> n;

    auto calc = [&](int x) {
        int sum = 0;
        std::string s = std::to_string(x);
        for (auto c : s) {
            int u = c - '0';
            sum += u * u;
        }
        return sum;
    };

    std::map<int, int> cnt;
    int ans = 0;
    for (int i = 0; i < n; i++) {
        int a;
        std::cin >> a;
        for (int _ = 0; _ < 100; _++) {
            a = calc(a);
        }
        ans += cnt[a]++;
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
