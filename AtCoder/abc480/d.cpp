#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N, Q;
    std::cin >> N >> Q;

    std::vector<int> A(N);
    for (int i = 0; i < N; i++) {
        std::cin >> A[i];
    }

    std::vector<int> S(N + 1), SS(N + 1);
    for (int i = 0; i < N; i++) {
        S[i + 1] = S[i] + A[i];
    }
    for (int i = 1; i <= N; i++) {
        SS[i] = SS[i - 1] + S[i];
    }

    // for (int i = 0; i <= N; i++) {
    //     std::cout << S[i] << " \n"[i == N];
    // }
    // for (int i = 0; i <= N; i++) {
    //     std::cout << SS[i] << " \n"[i == N];
    // }

    auto sum = [&](int x) {
        int lo = 0, hi = N;
        while (lo < hi) {
            int m = (lo + hi + 1) >> 1;
            if (x >= m * (1 + m) / 2) {
                lo = m;
            } else {
                hi = m - 1;
            }
        }
        
        return SS[lo] + S[x - lo * (1 + lo) / 2];
    };

    while (Q--) {
        int L, R;
        std::cin >> L >> R;
        L--, R--;
        // std::cout << sum(R + 1) << " " << sum(L) << " ";
        std::cout << sum(R + 1) - sum(L) << "\n";
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
