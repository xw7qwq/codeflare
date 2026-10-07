#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N;
    std::cin >> N;

    std::vector<int> A(N), B(N);
    for (int i = 0; i < N; i++) {
        std::cin >> A[i];
    }
    for (int i = 0; i < N; i++) {
        std::cin >> B[i];
    }

    int p = -1;
    for (int i = 0; i < N; i++) {
        if (A[i] > B[i]) {
            p = i;
            break;
        }
    }

    if (p == -1) {
        std::cout << "No\n";
        return;
    }

    std::cout << "Yes\n";
    for (int i = 0; i < N; i++) {
        if (i == p) {
            std::cout << (int)1E18;
        } else {
            std::cout << 1;
        }
        std::cout << " \n"[i == N - 1];
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
