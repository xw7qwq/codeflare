#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N;
    std::cin >> N;

    std::multiset<int> S;
    for (int i = 0; i < N; i++) {
        int a;
        std::cin >> a;
        S.insert(a);
        if (i >= 2) {
            auto it = S.rbegin();
            it++;
            it++;
            std::cout << *it << "\n";
        }
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
