#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N, K;
    std::cin >> N >> K;

    std::vector<int> a(N);
    for (int i = 0; i < N; i++) {
        std::cin >> a[i];
    }

    auto na = a;
    std::sort(na.begin(), na.end());
    int p = N;
    for (int i = 0; i < N; i++) {
        if (a[i] != na[i]) {
            p = i;
            break;
        }
    }

    for (int i = p + K; i < N; i++) {
        if (a[i] != na[i]) {
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
