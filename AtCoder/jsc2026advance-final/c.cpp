#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N, Q;
    std::cin >> N >> Q;

    std::vector<int> a(N);
    for (int i = 0; i < N; i++) {
        std::cin >> a[i];
        a[i]--;
    }

    std::vector<int> ask(Q);
    for (int i = 0; i < Q; i++) {
        std::cin >> ask[i];
        ask[i]--;
    }

    std::vector<bool> has(N);
    std::vector<int> ans;
    for (int i = Q - 1; i >= 0; i--) {
        if (!has[ask[i]]) {
            has[ask[i]] = true;
            ans.push_back(ask[i]);
        }
    }
    for (int i = N - 1; i >= 0; i--) {
        if (!has[a[i]]) {
            ans.push_back(a[i]);
        }
    }

    std::reverse(ans.begin(), ans.end());

    for (int i = 0; i < N; i++) {
        std::cout << ans[i] + 1 << " \n"[i == N - 1];
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
