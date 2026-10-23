#include <bits/stdc++.h>

#define int long long

using i64 = long long;
using u64 = unsigned long long;

void solve() {
    int H, W;
    std::cin >> H >> W;

    std::vector<std::string> g(H);
    for (int i = 0; i < H; i++) {
        std::cin >> g[i];
    }

    constexpr int inf = 1E7 + 1;
    std::vector cnt(H, std::vector<int>(W, inf));
    std::queue<std::pair<int, int>> q;
    int dx[] = {0, 0, -1, -1, -1, 1, 1, 1}, dy[] = {1, -1, 1, 0, -1, 1, 0, -1};
    for (int x = 0; x < H; x++) {
        for (int y = 0; y < W; y++) {
            if (g[x][y] == '#') {
                for (int i = 0; i < 8; i++) {
                    int nx = x + dx[i], ny = y + dy[i];
                    if (nx >= 0 && nx < H && ny >= 0 && ny < W) {
                        if (cnt[nx][ny] == inf && g[nx][ny] == '.') {
                            cnt[nx][ny] = 1;
                            q.push({nx, ny});
                        }
                    }
                }
            }
        }
    }
    
    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();

        for (int i = 0; i < 8; i++) {
            int nx = x + dx[i], ny = y + dy[i];
            if (nx >= 0 && nx < H && ny >= 0 && ny < W) {
                if (cnt[nx][ny] == inf) {
                    cnt[nx][ny] = cnt[x][y] + 1;
                    q.push({nx, ny});
                }
            }
        }
    }

    for (int x = 0; x < H; x++) {
        for (int y = 0; y < W; y++) {
            if (cnt[x][y] % 2 == 0) {
                std::cout << '#';
            } else {
                std::cout << '.';
            }
        }
        std::cout << "\n";
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
