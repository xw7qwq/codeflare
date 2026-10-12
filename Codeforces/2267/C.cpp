#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

std::vector<int> minp, primes;

void sieve(int n) {
    minp.assign(n + 1, 0);
    primes.clear();
    
    for (int i = 2; i <= n; i++) {
        if (minp[i] == 0) {
            minp[i] = i;
            primes.push_back(i);
        }
        
        for (auto p : primes) {
            if (i * p > n) {
                break;
            }
            minp[i * p] = p;
            if (p == minp[i]) {
                break;
            }
        }
    }
}

bool isprime(int n) {
    return minp[n] == n;
}

void solve() {
    int n, x;
    std::cin >> n >> x;

    std::vector<int> a(n);
    for (int i = 0; i < n; i++) {
        std::cin >> a[i];
    }

    std::vector<int> ps;
    for (auto p : primes) {
        if (p > x) {
            break;
        }
        if (x % p == 0) {
            ps.push_back(p);
            while (x % p == 0) {
                x /= p;
            }
        }
    }

    int ans = 0;
    for (auto p : ps) {
        int res = 0;
        for (auto u : a) {
            if (u % p == 0) {
                res += u;
            }
        }
        ans = std::max(ans, res);
    }

    std::cout << ans << "\n";
}

signed main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    sieve(4E5);

    int t = 1;
    std::cin >> t;
    while (t--) {
        solve();
    }

    return 0;
}
