#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    std::string s;
    std::getline(std::cin, s);
    std::set<char> set;
    for (auto c : s) {
        if (std::islower(c)) {
            // std::cout << c << " ";
            set.insert(c);
        }
    }
    std::cout << set.size() << "\n";
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
