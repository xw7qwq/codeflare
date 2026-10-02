#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int N, Q;
    std::cin >> N >> Q;

    std::vector<char> ans(N, '?');
    std::vector<int> op(Q), X(Q);
    std::vector<char> C(Q);

    std::vector<int> st(N);
    for (int i = 0; i < Q; i++) {
        std::cin >> op[i];
        if (op[i] == 1) {
            std::cin >> X[i];
            X[i]--;
            st[X[i]] ^= 1;
        } else {
            std::cin >> C[i];
        }
    }

    std::set<int> s;
    for (int i = 0; i < N; i++) {
        if (st[i] == 0) {
            s.insert(i);
        }
    }
    
    for (int i = Q - 1; i >= 0; i--) {
        if (op[i] == 1) {
            st[X[i]] ^= 1;
            if (st[X[i]] == 0) {
                if (ans[X[i]] == '?') {
                    s.insert(X[i]);
                }
            } else {
                s.erase(X[i]);
            }
        } else {
            auto it = s.begin();
            while (it != s.end()) {
                ans[*it] = C[i];
                it = s.erase(it);
            }
        }
    }

    for (int i = 0; i < N; i++) {
        std::cout << (ans[i] == '?' ? 'a' : ans[i]);
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
