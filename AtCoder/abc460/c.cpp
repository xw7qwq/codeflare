#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N, M;
    std::cin >> N >> M;

    std::vector<int> A(N), B(M);
    for (int i = 0; i < N; i++) {
        std::cin >> A[i];
    }
    for (int i = 0; i < M; i++) {
        std::cin >> B[i];
    }

    std::sort(A.begin(), A.end(), std::greater<>());
    std::sort(B.begin(), B.end(), std::greater<>());

    int cnt = 0;
    for (int l = 0, r = 0; l < N && r < M; ) {
        while (r < M && 2 * A[l] < B[r]) {
            r++;
        }
        if (r != M) {
            cnt++;
            l++;
            r++;
        }
    }
    std::cout << cnt << "\n";
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
