#include <bits/stdc++.h>
using namespace std;

#ifdef _RUTHEN
#include <debug.hpp>
#else
#define show(...) true
#endif

#define rep(i, n) for (int i = 0; i < (n); i++)

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

#include "data_structure/segment_tree.hpp"
#include "algebra/monoid_min_index.hpp"

#include "data_structure/unionfind.hpp"

constexpr int dx[8] = {1, 0, -1, 0, 1, -1, -1, 1};
constexpr int dy[8] = {0, 1, 0, -1, 1, 1, -1, -1};
constexpr int INF = 1 << 30;

struct Pos {
    i32 x, y;

    Pos() = default;
    Pos(int x, int y) : x(x), y(y) {}
};
ostream& operator<<(ostream& os, const Pos& p) { return os << "(" << p.x << "," << p.y << ")"; }

enum class Response { not_broken, broken, finish, invalid };

inline i32 mdis(const Pos& p1, const Pos& p2) {
    // show(p1, p2);
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

struct Solver {
    const int N = 200;
    const int N2 = 200 * 200;
    int W, K, C;
    vector<Pos> source_pos, house_pos;

    vector<vector<int>> is_broken;
    int total_cost;

    using S_t = double;

    vector<Pos> house_near_pos;
    vector<int> house_connected;
    vector<vector<S_t>> S_allinfo;
    vector<vector<int>> is_source;

    UnionFind uf;

    Solver(int W, int K, int C, const vector<Pos>& source_pos, const vector<Pos>& house_pos)
        : W(W),
          K(K),
          C(C),
          source_pos(source_pos),
          house_pos(house_pos),  //
          is_broken(N, vector<int>(N, 0)),
          S_allinfo(N, vector<S_t>(N, 100)),
          total_cost(0),
          house_near_pos(K),
          house_connected(K, 0),
          is_source(N, vector<int>(N, 0)),
          uf(N2 + 1) {
        for (int i = 0; i < K; i++) {
            house_near_pos[i] = source_pos[0];
            for (int j = 1; j < W; j++) {
                if (mdis(house_near_pos[i], house_pos[i]) > mdis(source_pos[j], house_pos[i])) {
                    house_near_pos[i] = source_pos[j];
                }
            }
        }
        for (int i = 0; i < W; i++) {
            is_source[source_pos[i].x][source_pos[i].y] = 1;
        }
        // show(house_near_pos);
    }

    void solve() {
        for (int tt = 0; tt < K; tt++) {
            int ind = -1;
            for (int i = 0; i < K; i++) {
                if (house_connected[i]) continue;
                if (ind == -1) {
                    ind = i;
                } else if (mdis(house_near_pos[i], house_pos[i]) < mdis(house_near_pos[ind], house_pos[ind])) {
                    ind = i;
                }
            }
            // process
            house_connected[ind] = 1;
            move(house_pos[ind], house_near_pos[ind]);
            if (tt == K - 2) {
                // show(S_allinfo);
            }
        }

        // assert(false);
    }

    void move(const Pos& start, const Pos& goal) {
        // make function for comment cout?
        cout << "# move from (" << start.x << "," << start.y << ") to (" << goal.x << "," << goal.y << ")" << endl;
        constexpr int N2 = 200 * 200;  // N * N
        vector<pair<int, int>> init(N2);
        vector<vector<int>> g(N, vector<int>(N)), h(N, vector<int>(N));
        rep(i, N) {
            rep(j, N) {
                g[i][j] = mdis(Pos(i, j), start) * 100;
                h[i][j] = mdis(Pos(i, j), goal) * 100;
                if (is_broken[i][j]) {
                    init[i * N + j] = {INF, i * N + j};
                } else {
                    init[i * N + j] = {g[i][j] + h[i][j] + S_allinfo[i][j], i * N + j};
                }
            }
        }
        SegmentTree<MonoidMinIndex<int>> seg(init);
        while (true) {
            auto [f, inj] = seg.all_prod();
            int i = inj / N, j = inj % N;
            if (is_broken[i][j]) {
            }
            int power_res = destruct(i, j);
            if (power_res != 0) {
                S_allinfo[i][j] = power_res;
            }
            rep(k, 4) {
                int nx = i + dx[k], ny = j + dy[k];
                if (is_broken[nx][ny]) {
                    uf.merge(nx * N + ny, inj);
                }
            }
            if (is_source[i][j]) {
                uf.merge(inj, N2);
            }
            if (uf.same(start.x * N + start.y, N2)) {
                // already broken
                break;
            }
            // update
            seg.set(inj, {INF, inj});
            rep(k, 8) {
                int nx = i + dx[k], ny = j + dy[k];
                if (!(0 <= nx and nx < N and 0 <= ny and ny < N)) continue;
                if (is_broken[nx][ny]) continue;
                int s_around_sum = 0, s_around_count = 0;
                rep(l, 8) {
                    int rx = nx + dx[l], ry = ny + dy[l];
                    if (!(0 <= rx and rx < N and 0 <= ry and ry < N)) continue;
                    s_around_sum += S_allinfo[rx][ry];
                    s_around_count++;
                }
                S_allinfo[nx][ny] = S_t(s_around_sum) / s_around_count;
                // show(S_allinfo[nx][ny]);
                g[nx][ny] = min(g[nx][ny], f - h[i][j]);
                init[nx * N + ny] = {g[nx][ny] + h[nx][ny] + S_allinfo[nx][ny], nx * N + ny};
            }
        }

        //// x
        // if (start.x < goal.x) {
        //    for (int x = start.x; x < goal.x; x++) {
        //        destruct(x, start.y);
        //    }
        //} else {
        //    for (int x = start.x; x > goal.x; x--) {
        //        destruct(x, start.y);
        //    }
        //}
        //// y
        // if (start.y < goal.y) {
        //    for (int y = start.y; y <= goal.y; y++) {
        //        destruct(goal.x, y);
        //    }
        //} else {
        //    for (int y = start.y; y >= goal.y; y--) {
        //        destruct(goal.x, y);
        //    }
        //}
    }

    int destruct(int x, int y) {
        int current_power_sum = 0;
        const int power = 10;
        while (!is_broken[x][y]) {
            Response result = query(x, y, power);
            if (result == Response::finish) {
                // cerr << "total_cost=" << total_cost << endl;
                exit(0);
            } else if (result == Response::invalid) {
                // cerr << "invalid: y=" << y << " x=" << x << endl;
                exit(1);
            }
            current_power_sum += power;
            // power = min(power * 2, 5000 - current_power_sum);
            // power = min(power, 5000 - current_power_sum);
        }
        for (int i = 0; i < K; i++) {
            if (house_connected[i]) continue;
            if (mdis(house_near_pos[i], house_pos[i]) > mdis(Pos(x, y), house_pos[i])) {
                house_near_pos[i] = Pos(x, y);
            }
        }
        S_allinfo[x][y] = current_power_sum;
        return current_power_sum;
    }

    Response query(int x, int y, int power) {
        total_cost += power + C;
        cout << x << ' ' << y << ' ' << power << endl;  // endl does flush
        int r;
        cin >> r;
        switch (r) {
            case 0:
                return Response::not_broken;
            case 1:
                is_broken[x][y] = 1;
                return Response::broken;
            case 2:
                is_broken[x][y] = 1;
                return Response::finish;
            default:
                return Response::invalid;
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    // input
    int N, W, K, C;
    cin >> N >> W >> K >> C;
    vector<Pos> source_pos(W);
    vector<Pos> house_pos(K);
    for (int i = 0; i < W; i++) cin >> source_pos[i].x >> source_pos[i].y;
    for (int i = 0; i < K; i++) cin >> house_pos[i].x >> house_pos[i].y;
    // show(N, W, K, C, source_pos, house_pos);

    // solver
    Solver solver(W, K, C, source_pos, house_pos);
    solver.solve();
    return 0;
}