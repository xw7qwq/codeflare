#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    std::cin >> n;

    std::string s;
    std::cin >> s;

    std::vector<int> cnt(2);
    for (int l = 0, r = 0; l < n; l = r) {
        while (r < n && s[l] == s[r]) {
            r++;
        }
        cnt[s[l] - '0'] += r - l - 1;
    }

    if (std::abs(cnt[0] - cnt[1]) <= 1) {
        std::cout << cnt[0] + cnt[1] << "\n";
        return;
    }
    cnt[s.front() - '0']++;
    if (std::abs(cnt[0] - cnt[1]) <= 1) {
        std::cout << cnt[0] + cnt[1] << "\n";
        return;
    }
    cnt[s.front() - '0']--;

    cnt[s.back() - '0']++;
    if (std::abs(cnt[0] - cnt[1]) <= 1) {
        std::cout << cnt[0] + cnt[1] << "\n";
        return;
    }
    cnt[s.back() - '0']--;

    cnt[s.front() - '0']++;
    cnt[s.back() - '0']++;
    if (std::abs(cnt[0] - cnt[1]) <= 1) {
        std::cout << cnt[0] + cnt[1] << "\n";
        return;
    }
    cnt[s.front() - '0']--;
    cnt[s.back() - '0']--;

    std::cout << -1 << "\n";
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
