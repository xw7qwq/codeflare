#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int x, y, p, q;
    std::cin >> x >> y >> p >> q;

    int cur = 0;
    std::map<std::tuple<std::pair<int, int>, std::pair<int, int>, int>, int> mp; // (xa, ya), (xb, yb), 0/1
    std::map<int, std::tuple<std::pair<int, int>, std::pair<int, int>, int>> invmp;
    std::vector<int> vis;
    auto is = [&](std::pair<int, int> a, std::pair<int, int> b, int o) -> int{
        if (mp.contains({a, b, o})) {
            return mp[{a, b, o}];
        }
        mp[{a, b, o}] = cur;
        invmp[cur] = {a, b, o};
        vis.push_back(0);
        return cur++;
    };

    auto invis = [&](int x) {
        return invmp[x];
    };
    
    std::queue<std::pair<int, int>> pq;
    int id = is({x, y}, {0, 0}, 1);
    pq.push({0, id});
    vis[id] = 1;
    while (!pq.empty()) {
        auto [w, _] = pq.front();
        auto [a, b, o] = invis(_);
        auto [ax, ay] = a;
        auto [bx, by] = b;
        pq.pop();

        if (ax == 0 && o == 0) {
            std::cout << w << "\n";
            return;
        }

        for (int sum = std::min(p, (o ? ax + ay : bx + by)); sum >= 0; sum--) {
            for (int dx = std::min(sum, (o ? ax : bx)); dx >= std::max(0LL, sum - (o ? ay : by)); dx--) {
                int dy = sum - dx;
                if (o) {
                    int nax = ax - dx, nay = ay - dy;
                    int nbx = bx + dx, nby = by + dy;
                    if (nax > 0 && nay > nax + q) {
                        continue;
                    }
                    int id = is({nax, nay}, {nbx, nby}, 0);
                    if (!vis[id]) {
                        vis[id] = 1;
                        pq.push({w + 1, id});
                    }
                } else {
                    int nbx = bx - dx, nby = by - dy;
                    int nax = ax + dx, nay = ay + dy;
                    if (nbx > 0 && nby > nbx + q) {
                        continue;
                    }
                    int id = is({nax, nay}, {nbx, nby}, 1);
                    if (!vis[id]) {
                        vis[id] = 1;
                        pq.push({w + 1, id});
                    }
                }
            }
        }
    }
    std::cout << "-1\n";
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
