#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int n;
    std::cin >> n;

    int cnt = 0;
    for (int i = 0; i < n; i++) {
        std::string s;
        std::cin >> s;
        if (s == "Tetrahedron") {
            cnt += 4;
        } else if (s == "Cube") {
            cnt += 6;
        } else if (s == "Octahedron") {
            cnt += 8;
        } else if (s == "Dodecahedron") {
            cnt += 12;
        } else {
            cnt += 20;
        }
    }

    std::cout << cnt << "\n";
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
