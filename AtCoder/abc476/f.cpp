#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

using i128 = __int128;

void solve() {
    int N, M;
    std::cin >> N >> M;

    std::vector<int> A(N), B(N);
    for (int i = 0; i < N; i++) {
        std::cin >> A[i];
    }
    for (int i = 0; i < N; i++) {
        std::cin >> B[i];
    }

    std::vector g(N, std::vector<int>(N));
    std::vector<int> sx(2 * N - 1), sy(2 * N - 1);
    int offy = N - 1;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            g[i][j] = (A[i] * B[j]) % M;
            sx[i + j] += g[i][j];
            sy[i - j + offy] += g[i][j];
        }
    }

    std::vector<int> costx(2 * N - 1), costy(2 * N - 1);
    i128 pred = 0, sufd = 0, predx = 0, sufdx = 0;
    for (int i = 2 * N - 2; i >= 0; i--) {
        sufd += sx[i];
        sufdx += sx[i] * i;
    }
    for (int i = 0; i < 2 * N - 1; i++) {
        sufd -= sx[i];
        sufdx -= sx[i] * i;
        pred += sx[i];
        predx += sx[i] * i;
        costx[i] = i * pred - predx + sufdx - i * sufd;
    }

    pred = 0, sufd = 0;
    i128 predy = 0, sufdy = 0;
    for (int i = 2 * N - 2; i >= 0; i--) {
        sufd += sy[i];
        sufdy += sy[i] * i;
    }
    for (int i = 0; i < 2 * N - 1; i++) {
        sufd -= sy[i];
        sufdy -= sy[i] * i;
        pred += sy[i];
        predy += sy[i] * i;
        costy[i] = i * pred - predy + sufdy - i * sufd;
    }

    int ans = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int x = i + j, y = i - j + offy;
            int f = (costx[x] + costy[y]) / 2;
            // std::cout << f << " ";
            ans ^= f + i * N + j;
        }
        // std::cout << "\n";
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
