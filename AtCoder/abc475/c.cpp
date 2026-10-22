#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N, S, L;
    std::cin >> N >> S >> L;
    S--;

    std::vector<int> p(N);
    for (int i = 0; i < N; i++) {
        std::cin >> p[i];
    }

    std::vector<int> pre(N + 1);
    for (int i = 0; i < N; i++) {
        pre[i + 1] = pre[i] + p[i];
    }

    auto cost = [&](int l, int r) {
        int a = pre[S] - pre[l], b = pre[r] - pre[S];
        return a + b - std::min(a, b);
    };

    int mx = 0;
    for (int l = 0, r = S + 1; l < S; l++) {
        while (r < S && cost(l, r) <= L) {
            r++;
        }
        mx = std::max(mx, r - l - 1);
    }
    std::cout << mx << "\n";
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
