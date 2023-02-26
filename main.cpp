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

    UnionFind() = default;
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

    std::vector<std::vector<int>> groups() {
        int n = int(parents.size());
        std::vector<std::vector<int>> res(n);
        for (int i = 0; i < n; i++) res[leader(i)].push_back(i);
        res.erase(remove_if(res.begin(), res.end(), [&](const std::vector<int>& v) { return v.empty(); }), res.end());
        return res;
    }

    void init(int n) { parents.assign(n, -1); }  // reset
};

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using Double = double;
const Double EPS = 1e-8;
inline int sign(const Double& x) { return x <= -EPS ? -1 : (x >= EPS ? 1 : 0); }
inline bool equal(const Double& a, const Double& b) { return sign(a - b) == 0; }

constexpr int dx[8] = {1, 0, -1, 0, 1, -1, -1, 1};
constexpr int dy[8] = {0, 1, 0, -1, 1, 1, -1, -1};
constexpr int INF = 1 << 30;
constexpr int N2 = 200 * 200;

int S_PRED_INIT = 105;
int S_ONCE = 25;
Double S_PRED_LOWER_BOUND_PARAM = 0.8301;
Double S_DIFFERENTIAL_PARAM = 0.1413;
Double DIRECTION_CHANGE_FREQUENCY_C = 0.0234;
Double DIRECTION_CHANGE_FREQUENCY_MAG = 0.0758;
int DIRECTION_CHANGE_FREQUENCY_CONST = 6;

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
bool operator==(const Pos& a, const Pos& b) { return a.x == b.x and a.y == b.y; }

enum class Response { not_broken, broken, finish, invalid };

inline i32 mdis(const Pos& p1, const Pos& p2) { return abs(p1.x - p2.x) + abs(p1.y - p2.y); }

inline i32 udis(const Pos& p1, const Pos& p2) { return i32(sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y))); }

struct Solver {
    int N, W, K, C;
    vector<Pos> source_pos, house_pos;

    vector<vector<int>> is_broken;
    int total_cost;

    int x_lower_bound, y_lower_bound;
    int x_upper_bound, y_upper_bound;

    vector<vector<int>> s_current_power;      // 各Sijに現在与えたダメージ量の総和
    vector<vector<int>> is_source, is_house;  // それぞれ水源マス, 家マスか
    UnionFind uf;

    int magnification;  // マップの拡大率
    int direction_change_frequency;

    Solver(int N, int W, int K, int C, const vector<Pos>& source_pos, const vector<Pos>& house_pos)
        : N(N),
          W(W),
          K(K),
          C(C),
          source_pos(source_pos),
          house_pos(house_pos),  //
          is_broken(N, vector<int>(N, 0)),
          total_cost(0),
          x_lower_bound(N - 1),
          y_lower_bound(N - 1),
          x_upper_bound(0),
          y_upper_bound(0),
          s_current_power(N, vector<int>(N, 0)),
          is_source(N, vector<int>(N, 0)),
          is_house(N, vector<int>(N, 0)),
          uf(N2 + 1) {}

    void init() {
        for (int i = 0; i < W; i++) {
            x_lower_bound = min(x_lower_bound, source_pos[i].x);
            y_lower_bound = min(y_lower_bound, source_pos[i].y);
            x_upper_bound = max(x_upper_bound, source_pos[i].x);
            y_upper_bound = max(y_upper_bound, source_pos[i].y);
            is_source[source_pos[i].x][source_pos[i].y] = 1;
        }
        for (int i = 0; i < K; i++) {
            x_lower_bound = min(x_lower_bound, house_pos[i].x);
            y_lower_bound = min(y_lower_bound, house_pos[i].y);
            x_upper_bound = max(x_upper_bound, house_pos[i].x);
            y_upper_bound = max(y_upper_bound, house_pos[i].y);
            is_house[house_pos[i].x][house_pos[i].y] = 1;
        }
    }

    pair<int, int> predict_power(int x, int y) {
        Double s_pred_sum = 0;
        int s_pred_count = 0;
        rep(k, 8) {
            int nx = x + dx[k], ny = y + dy[k];
            if (!(0 <= nx and nx < N and 0 <= ny and ny < N)) continue;
            if (is_broken[nx][ny]) {
                s_pred_sum += s_current_power[nx][ny];
                s_pred_count++;
            }
        }
        Double s_pred = s_pred_count == 0 ? S_PRED_INIT : s_pred_sum / s_pred_count;
        int s_start = s_pred * S_PRED_LOWER_BOUND_PARAM;
        int s_differential = s_pred * S_DIFFERENTIAL_PARAM;
        s_start = max(1, min(s_start - s_current_power[x][y], 5000 - s_current_power[x][y]));
        return {s_start, s_differential};
    }

    void destruct_all(int x, int y) {
        if (is_broken[x][y]) return;
        auto [s_start, s_differential] = predict_power(x, y);
        int power = s_start;
        while (!is_broken[x][y]) {
            Response result = query(x, y, power);
            if (result == Response::finish) {
                // cerr << "total_cost=" << total_cost << endl;
                exit(0);
            } else if (result == Response::invalid) {
                cerr << "invalid: y=" << y << " x=" << x << endl;
                exit(1);
            }
            s_current_power[x][y] += power;
            power = max(1, min(s_differential, 5000 - s_current_power[x][y]));
        }
        unite_around(x, y);
    }

    void unite_around(int x, int y) {
        if (is_source[x][y]) uf.merge(N2, x * N + y);
        rep(k, 4) {
            int nx = x + dx[k], ny = y + dy[k];
            if (!(0 <= nx and nx < N and 0 <= ny and ny < N)) continue;
            if (is_broken[nx][ny]) {
                uf.merge(x * N + y, nx * N + ny);
            }
        }
    }

    void destruct_once(int x, int y, int power) {
        if (is_broken[x][y]) return;
        Response result = query(x, y, power);
        if (result == Response::finish) {
            // cerr << "total_cost=" << total_cost << endl;
            exit(0);
        } else if (result == Response::invalid) {
            cerr << "invalid: y=" << y << " x=" << x << endl;
            exit(1);
        }
        s_current_power[x][y] += power;
        if (is_broken[x][y]) unite_around(x, y);
    }

    void excavation_cross() {
        vector<int> cross_length;
        for (int i = 0; i < W; i++) {
            destruct_all(source_pos[i].x, source_pos[i].y);
            rep(k, 4) {
                int cx = source_pos[i].x + dx[k], cy = source_pos[i].y + dy[k];
                int current_length = 0;
                while (true) {
                    current_length++;
                    if (!(x_lower_bound <= cx and cx <= x_upper_bound)) break;
                    if (!(y_lower_bound <= cy and cy <= y_upper_bound)) break;
                    destruct_once(cx, cy, S_ONCE);
                    if (is_broken[cx][cy]) {
                        cx += dx[k], cy += dy[k];
                    } else {
                        break;
                    }
                }
                cross_length.push_back(current_length);
            }
        }
        for (int i = 0; i < K; i++) {
            destruct_all(house_pos[i].x, house_pos[i].y);
            rep(k, 4) {
                int cx = house_pos[i].x + dx[k], cy = house_pos[i].y + dy[k];
                int current_length = 0;
                while (true) {
                    current_length++;
                    if (!(x_lower_bound <= cx and cx <= x_upper_bound)) break;
                    if (!(y_lower_bound <= cy and cy <= y_upper_bound)) break;
                    destruct_once(cx, cy, S_ONCE);
                    if (is_broken[cx][cy]) {
                        cx += dx[k], cy += dy[k];
                    } else {
                        break;
                    }
                }
                cross_length.push_back(current_length);
            }
        }
        assert(cross_length.size() == 4 * (K + W));
        sort(cross_length.rbegin(), cross_length.rend());
        // set magnification, direction_chenge_frequency
        magnification = cross_length[2 * (K + W)];
        direction_change_frequency = DIRECTION_CHANGE_FREQUENCY_C * C + DIRECTION_CHANGE_FREQUENCY_MAG * magnification;
        direction_change_frequency += DIRECTION_CHANGE_FREQUENCY_CONST;
        direction_change_frequency = max(1, direction_change_frequency);
    }

    bool same(const Pos& a, const Pos& b) { return uf.same(a.x * N + a.y, b.x * N + b.y); }

    vector<vector<Pos>> calc_connected_pos(vector<Pos>& current_pos) {
        vector<vector<Pos>> connected_pos(current_pos.size());
        rep(k, current_pos.size()) {
            rep(i, N) {
                rep(j, N) {
                    if (same(current_pos[k], Pos(i, j))) {
                        connected_pos[k].push_back(Pos(i, j));
                    }
                }
            }
        }
        return connected_pos;
    }

    tuple<int, Pos, Pos> calc_closest_points(vector<Pos>& a, vector<Pos>& b) {
        int min_distance = INF;
        Pos argmin_pos1, argmin_pos2;
        for (auto&& ai : a) {
            for (auto&& bi : b) {
                if (min_distance > udis(ai, bi)) {
                    min_distance = udis(ai, bi);
                    argmin_pos1 = ai;
                    argmin_pos2 = bi;
                }
            }
        }
        return {min_distance, argmin_pos1, argmin_pos2};
    }

    void solve() {
        init();
        excavation_cross();
        while (true) {
            random_device seed_gen;
            mt19937 engine(seed_gen());
            shuffle(source_pos.begin(), source_pos.end(), engine);
            vector<Pos> source_current_pos;
            for (int i = 0; i < W; i++) {
                bool seen = false;
                for (auto p : source_current_pos) {
                    if (same(p, source_pos[i])) seen = true;
                }
                if (seen) continue;
                source_current_pos.push_back(source_pos[i]);
            }
            shuffle(house_pos.begin(), house_pos.end(), engine);
            vector<Pos> house_current_pos;
            for (int i = 0; i < K; i++) {
                bool seen = false;
                for (auto p : source_current_pos) {
                    if (same(p, house_pos[i])) seen = true;
                }
                for (auto p : house_current_pos) {
                    if (same(p, house_pos[i])) seen = true;
                }
                if (seen) continue;
                house_current_pos.push_back(house_pos[i]);
            }
            if (house_current_pos.size() == 0) break;
            // 点に対し、その点の連結成分を列挙
            auto source_connected_pos = calc_connected_pos(source_current_pos);
            auto house_connected_pos = calc_connected_pos(house_current_pos);

            int min_distance = INF;
            Pos argmin_pos1, argmin_pos2;
            // source and house
            rep(k, source_connected_pos.size()) {
                rep(k2, house_connected_pos.size()) {
                    auto [d, a, b] = calc_closest_points(source_connected_pos[k], house_connected_pos[k2]);
                    if (min_distance > d) {
                        min_distance = d;
                        argmin_pos1 = a;
                        argmin_pos2 = b;
                    }
                }
            }
            // house and house
            rep(k, house_connected_pos.size()) {
                rep(k2, k) {
                    auto [d, a, b] = calc_closest_points(house_connected_pos[k], house_connected_pos[k2]);
                    if (min_distance > d) {
                        min_distance = d;
                        argmin_pos1 = a;
                        argmin_pos2 = b;
                    }
                }
            }
            move(argmin_pos1, argmin_pos2);
        }

        // assert(false);
    }

    void move(const Pos& start, const Pos& goal) {
        // make function for comment cout?
        cout << "# move from (" << start.x << "," << start.y << ") to (" << goal.x << "," << goal.y << ")" << endl;
        int cx = start.x, cy = start.y;
        int move_count = 0;
        int dir = -1;
        while (!same(Pos(cx, cy), goal)) {
            move_count++;
            vector<int> direction_index, direction_distance;
            rep(k, 4) {
                int nx = cx + dx[k], ny = cy + dy[k];
                if (!(0 <= nx and nx < N and 0 <= ny and ny < N)) continue;
                if (mdis(Pos(cx, cy), goal) > mdis(Pos(nx, ny), goal)) {
                    direction_index.push_back(k);
                    direction_distance.push_back(abs(cx - goal.x) * abs(dx[k]) + abs(cy - goal.y) * abs(dy[k]));
                }
            }
            assert(direction_index.size() <= 2);
            int index;
            if (direction_index.size() == 1) {
                index = direction_index[0];
            } else {
                if (move_count % direction_change_frequency == 0) {
                    // direction chenge
                    vector<int> direction_s;
                    rep(k, 4) {
                        int nx = cx + dx[k], ny = cy + dy[k];
                        if (!(0 <= nx and nx < N and 0 <= ny and ny < N)) continue;
                        if (mdis(Pos(cx, cy), goal) > mdis(Pos(nx, ny), goal)) {
                            destruct_all(nx, ny);
                            direction_s.push_back(s_current_power[nx][ny]);
                        }
                    }
                    if (direction_s[0] < direction_s[1]) {
                        index = direction_index[0];
                        dir = direction_index[0];
                    } else {
                        index = direction_index[1];
                        dir = direction_index[1];
                    }
                } else {
                    if (dir == -1) {
                        index = my.nextDouble() < (Double)direction_distance[0] / mdis(Pos(cx, cy), goal) ? direction_index[0] : direction_index[1];
                        dir = index;
                    } else {
                        index = dir;
                    }
                }
            }
            int nx = cx + dx[index], ny = cy + dy[index];
            destruct_all(nx, ny);
            cx = nx, cy = ny;
        }
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

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(0);
    if (argc == 8) {
        S_PRED_INIT = atoi(argv[1]);
        S_ONCE = atoi(argv[2]);
        S_PRED_LOWER_BOUND_PARAM = atof(argv[3]);
        S_DIFFERENTIAL_PARAM = atof(argv[4]);
        DIRECTION_CHANGE_FREQUENCY_C = atof(argv[5]);
        DIRECTION_CHANGE_FREQUENCY_MAG = atof(argv[6]);
        DIRECTION_CHANGE_FREQUENCY_CONST = atoi(argv[7]);
    }
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