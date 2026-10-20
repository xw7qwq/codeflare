#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N, W;
    std::cin >> N >> W;

    std::vector<int> A(N), B(N);
    for (int i = 0; i < N; i++) {
        std::cin >> A[i] >> B[i];
    }

    int ans = 0;
    for (int t = 0; t < (1 << N); t++) {
        int w = 0, c = 0;
        for (int i = 0; i < N; i++) {
            if ((t >> i) & 1) {
                w += A[i];
                c += B[i];
            }
        }

        if (c <= W) {
            ans = std::max(ans, w);
        }
    }
    std::cout << ans << "\n";
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
