#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    std::string s;
    std::cin >> s;

    std::deque<char> q;
    int f = 0;
    for (auto c : s) {
        if (c == 'R') {
            f ^= 1;
        } else {
            if (f == 0) {
                q.push_back(c);
            } else {
                q.push_front(c);
            }
        }
    }

    std::vector<char> stk;
    for (auto c : q) {
        if (!stk.empty() && stk.back() == c) {
            stk.pop_back();
        } else {
            stk.push_back(c);
        }
    }

    if (f == 0) {
        for (int i = 0; i < stk.size(); i++) {
            std::cout << stk[i];
        }
    } else {
        for (int i = stk.size() - 1; i >= 0; i--) {
            std::cout << stk[i];
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
