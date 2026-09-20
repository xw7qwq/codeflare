#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N;
    std::cin >> N;

    std::string S, T;
    std::cin >> S >> T;

    for (int i = 0; i < N; i++) {
        if (S[i] != T[i] && T[i] != '*') {
            std::cout << "No\n";
            return;
        }
    }
    std::cout << "Yes\n";
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
