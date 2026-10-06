#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

constexpr int P = 1E9 + 7;

i64 mul(i64 a, i64 b, i64 p) {
    i64 c = a * b - i64(1.0L * a * b / p) * p;
    c %= p;
    if (c < 0) {
        c += p;
    }
    return c;
}

i64 power(i64 a, i64 b, i64 p) {
    i64 res = 1;
    for (; b; b /= 2, a = mul(a, a, p)) {
        if (b % 2) {
            res = mul(res, a, p);
        }
    }
    return res;
}

void solve() {
    int n, p;
    std::cin >> n >> p;

    std::cout << (p - 1) * power(p - 2, n - 1, P) % P << "\n";
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
