#include <bits/stdc++.h>
using namespace std;

#ifdef _RUTHEN
#include <debug.hpp>
#else
#define show(...) true
#endif

#define rep(i, n) for (int i = 0; i < (n); i++)

struct UnionFind {
    std::vector<int> parents;

    UnionFind() {}
    UnionFind(int n) : parents(n, -1) {}

    int leader(int x) { return parents[x] < 0 ? x : parents[x] = leader(parents[x]); }

    bool merge(int x, int y) {
        x = leader(x), y = leader(y);
        if (x == y) return false;
        if (parents[x] > parents[y]) std::swap(x, y);
        parents[x] += parents[y];
        parents[y] = x;
        return true;
    }

    bool same(int x, int y) { return leader(x) == leader(y); }

    int size(int x) { return -parents[leader(x)]; }

    void init(int n) { parents.assign(n, -1); }  // reset
};

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using Double = double;
const Double EPS = 1e-8;
inline int sign(const Double& x) { return x <= -EPS ? -1 : (x >= EPS ? 1 : 0); }
inline bool equal(const Double& a, const Double& b) { return sign(a - b) == 0; }

constexpr int dx[8] = {1, 0, -1, 0, 1, -1, -1, 1};
constexpr int dy[8] = {0, 1, 0, -1, 1, 1, -1, -1};
vector<vector<int>> permutation_4;
constexpr int INF = 1 << 30;
const int N2 = 200 * 200;

constexpr int S_PRED_INIT = 30;
const Double S_PRED_LOWER_BOUND_PARAM = 0.6;
const Double S_DIFFERENTIAL_PARAM = 0.2;

// Random
const int MAX_RAND = 1 << 30;
struct Rand {
    i64 x, y, z, w, o;
    Rand() {}
    Rand(i64 seed) {
        reseed(seed);
        o = 0;
    }
    inline void reseed(i64 seed) {
        x = 0x498b3bc5 ^ seed;
        y = 0;
        z = 0;
        w = 0;
        rep(i, 20) mix();
    }
    inline void mix() {
        i64 t = x ^ (x << 11);
        x = y;
        y = z;
        z = w;
        w = w ^ (w >> 19) ^ t ^ (t >> 8);
    }
    inline i64 rand() {
        mix();
        return x & (MAX_RAND - 1);
    }
    inline int nextInt(int n) { return rand() % n; }
    inline int nextInt(int L, int R) { return rand() % (R - L + 1) + L; }
    inline int nextBool() {
        if (o < 4) o = rand();
        o >>= 2;
        return o & 1;
    }
    double nextDouble() { return rand() * 1.0 / MAX_RAND; }
};
Rand my(2023);

struct Pos {
    i32 x, y;

    Pos() = default;
    Pos(int x, int y) : x(x), y(y) {}
};
ostream& operator<<(ostream& os, const Pos& p) { return os << "(" << p.x << "," << p.y << ")"; }

enum class Response { not_broken, broken, finish, invalid };

inline i32 mdis(const Pos& p1, const Pos& p2) { return abs(p1.x - p2.x) + abs(p1.y - p2.y); }

struct Solver {
    int N, W, K, C;
    vector<Pos> source_pos, house_pos;

    vector<vector<int>> is_broken;
    int total_cost;

    vector<Pos> house_near_pos;   // 各家からもっとも近い水のある地点
    vector<int> house_connected;  // 各家が既に水と繋がっているか
    vector<vector<int>> s_predicted;

    UnionFind uf;
    vector<vector<int>> is_source;

    Solver(int N, int W, int K, int C, const vector<Pos>& source_pos, const vector<Pos>& house_pos)
        : N(N),
          W(W),
          K(K),
          C(C),
          source_pos(source_pos),
          house_pos(house_pos),  //
          is_broken(N, vector<int>(N, 0)),
          total_cost(0),
          house_near_pos(K),
          house_connected(K, 0),
          s_predicted(N, vector<int>(N, -1)),
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
        show(house_near_pos);
    }

    void solve() {
        while (true) {
            int ind = -1;
            for (int i = 0; i < K; i++) {
                if (house_connected[i]) continue;
                if (ind == -1) {
                    ind = i;
                } else if (mdis(house_near_pos[i], house_pos[i]) < mdis(house_near_pos[ind], house_pos[ind])) {
                    ind = i;
                }
            }

            if (ind == -1) {
                break;
            }
            // process
            house_connected[ind] = 1;
            move(house_pos[ind], house_near_pos[ind]);
        }

        // assert(false);
    }

    void move(const Pos& start, const Pos& goal) {
        // make function for comment cout?
        cout << "# move from (" << start.x << "," << start.y << ") to (" << goal.x << "," << goal.y << ")" << endl;
        destruct(start.x, start.y);
        destruct(goal.x, goal.y);
        int cx = start.x, cy = start.y;
        while (!uf.same(cx * N + cy, N2)) {
            array<pair<int, Pos>, 4> score_pos;
            int score_pos_size = 0;
            int p4_index = my.nextInt(24);

            rep(k, 4) {
                int nx = cx + dx[permutation_4[p4_index][k]], ny = cy + dy[permutation_4[p4_index][k]];
                if (!(0 <= nx and nx < N and 0 <= ny and ny < N)) continue;
                if (mdis(Pos(nx, ny), goal) > mdis(Pos(cx, cy), goal)) continue;
                destruct(nx, ny);
                score_pos[score_pos_size++] = {s_predicted[nx][ny], Pos(nx, ny)};
            }
            int ind = 0;
            rep(i, score_pos_size) {
                if (score_pos[i].first < score_pos[ind].first) {
                    ind = i;
                }
            }
            cx = score_pos[ind].second.x;
            cy = score_pos[ind].second.y;
        }
    }

    void destruct(int x, int y) {
        if (is_broken[x][y]) return;
        Double s_pred_sum = 0;
        int s_pred_count = 0;
        rep(k, 8) {
            int nx = x + dx[k], ny = y + dy[k];
            if (!(0 <= nx and nx < N and 0 <= ny and ny < N)) continue;
            if (is_broken[nx][ny]) {
                s_pred_sum += s_predicted[nx][ny];
                s_pred_count++;
            }
        }

        Double s_pred = s_pred_count == 0 ? S_PRED_INIT : s_pred_sum / s_pred_count;
        int s_start = s_pred * S_PRED_LOWER_BOUND_PARAM;
        int s_differential = s_pred * S_DIFFERENTIAL_PARAM;
        int power = s_start;
        int current_power_sum = 0;
        while (!is_broken[x][y]) {
            Response result = query(x, y, power);
            if (result == Response::finish) {
                // cerr << "total_cost=" << total_cost << endl;
                exit(0);
            } else if (result == Response::invalid) {
                cerr << "invalid: y=" << y << " x=" << x << endl;
                exit(1);
            }
            current_power_sum += power;
            power = s_differential;
            power = min(power, 5000 - current_power_sum);
        }
        s_predicted[x][y] = current_power_sum;
        for (int i = 0; i < K; i++) {
            if (house_connected[i]) continue;
            if (mdis(house_near_pos[i], house_pos[i]) > mdis(Pos(x, y), house_pos[i])) {
                house_near_pos[i] = Pos(x, y);
            }
        }
        if (is_source[x][y]) {
            uf.merge(x * N + y, N2);
        }
        rep(k, 4) {
            int nx = x + dx[k], ny = y + dy[k];
            if (!(0 <= nx and nx < N and 0 <= ny and ny < N)) continue;
            if (is_broken[nx][ny]) {
                uf.merge(x * N + y, nx * N + ny);
            }
        }
        return;
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
    // generate permutation
    vector<int> p(4);
    iota(p.begin(), p.end(), 0);
    do {
        permutation_4.push_back(p);
    } while (next_permutation(p.begin(), p.end()));

    // input
    int N, W, K, C;
    cin >> N >> W >> K >> C;
    vector<Pos> source_pos(W);
    vector<Pos> house_pos(K);
    for (int i = 0; i < W; i++) cin >> source_pos[i].x >> source_pos[i].y;
    for (int i = 0; i < K; i++) cin >> house_pos[i].x >> house_pos[i].y;
    // show(N, W, K, C, source_pos, house_pos);

    // solver
    Solver solver(N, W, K, C, source_pos, house_pos);
    solver.solve();
    return 0;
}