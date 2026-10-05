#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    char c;
    std::string s;
    std::cin >> n >> c >> s;

    int cnt = 0;
    for (int i = 0; i < n / 2; i++) {
        int j = n - 1 - i;
        if (s[i] == s[j]) {
            continue;
        } else if (s[i] == c || s[j] == c) {
            cnt++;
        } else {
            cnt += 2;
        }
    }
    std::cout << cnt << "\n";
}

signed main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int t = 1;
    std::cin >> t;
    while (t--) {
        solve();
    }

    return 0;
}
