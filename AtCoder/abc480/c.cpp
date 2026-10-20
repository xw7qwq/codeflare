#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N, K, T;
    std::cin >> N >> K >> T;

    std::vector<int> A;
    for (int i = 0; i < N; i++) {
        int x;
        std::cin >> x;

        if (x < T) {
            A.push_back(x);
        }
    }

    std::sort(A.begin(), A.end());

    int cnt = 0;

    int c = 0;
    for (int i = 0; i < A.size(); i++) {
        if (c >= A[i]) {
            c = std::max(c, A[i] + K);
            continue;
        } else {
            int x = (A[i] - c + K - 1) / K;
            c = A[i] + K;
            cnt += x;
        }
    }
    if (c < T) {
        int x = (T - c + K - 1) / K;
        cnt += x;
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
