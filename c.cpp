#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve1() {
    int n;
    std::cin >> n;

    std::string A, B;
    std::cin >> A >> B;

    auto BB = B + B;
    int s = BB.find(A);
    std::cout << s << "\n";
}

void solve2() {
    int s;
    std::cin >> s;

    std::string B;
    std::cin >> B;

    int q;
    std::cin >> q;

    for (int i = 0; i < q; i++) {
        int x;
        std::cin >> x;
        // x--;
        // std::cout << x << " " << s << " ";
        std::cout << B[(x + s) % B.size()];
    }
    std::cout << "\n";
}

signed main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int op;
    std::cin >> op;

    if (op == 1) {
        solve1();
    } else {
        solve2();
    }

    return 0;
}
