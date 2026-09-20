#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N, M, K;
    std::cin >> N >> M >> K;

    int X, Y;
    std::cin >> X >> Y;

    std::vector<int> A(N), B(M);
    for (int i = 0; i < N; i++) {
        std::cin >> A[i];
    }
    for (int i = 0; i < M; i++) {
        std::cin >> B[i];
    }

    std::sort(A.begin(), A.end());
    std::vector<int> pre(N + 1);
    for (int i = 0; i < N; i++) {
        pre[i + 1] = pre[i] + A[i];
    }

    std::vector<std::pair<int, int>> p(M);
    for (int i = 0; i < M; i++) {
        p[i].first = (B[i] + K - 1) / K;
        p[i].second = p[i].first * K - B[i];
    }

    std::sort(p.begin(), p.end(), [&](auto a, auto b) {
        if (a.first != b.first) {
            return a.first < b.first;
        } else {
            return a.second > b.second;
        }
    });
    
    int S = X + Y * K;
    int mx = std::upper_bound(pre.begin(), pre.end(), S) - pre.begin() - 1;
    for (int i = 0; i < M; i++) {
        if (Y >= p[i].first) {
            Y -= p[i].first;
            S += p[i].second - p[i].first * K;
            int res = std::upper_bound(pre.begin(), pre.end(), S) - pre.begin() - 1;
            mx = std::max(mx, res + i + 1);
        } else {
            break;
        }
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
