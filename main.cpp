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

struct Pos {
    i32 x, y;
};
ostream& operator<<(ostream& os, const Pos& p) { return os << "(" << p.x << "," << p.y << ")"; }

enum class Response { not_broken, broken, finish, invalid };

struct Solver {
    int N, W, K, C;
    vector<Pos> source_pos, house_pos;

    vector<vector<int>> is_broken;
    int total_cost;

    Solver(int N, int W, int K, int C, const vector<Pos>& source_pos, const vector<Pos>& house_pos)
        : N(N),
          W(W),
          K(K),
          C(C),
          source_pos(source_pos),
          house_pos(house_pos),  //
          is_broken(N, vector<int>(N, 0)),
          total_cost(0) {}

    void solve() {
        for (auto&& house : house_pos) {
            move(house, source_pos[0]);
        }

        assert(false);
    }

    void move(const Pos& start, const Pos& goal) {
        // make function for comment cout?
        cout << "# move from (" << start.x << "," << start.y << ") to (" << goal.x << "," << goal.y << ")" << endl;

        // x
        if (start.x < goal.x) {
            for (int x = start.x; x < goal.x; x++) {
                destruct(x, start.y);
            }
        } else {
            for (int x = start.x; x > goal.x; x--) {
                destruct(x, start.y);
            }
        }
        // y
        if (start.y < goal.y) {
            for (int y = start.y; y <= goal.y; y++) {
                destruct(goal.x, y);
            }
        } else {
            for (int y = start.y; y >= goal.y; y--) {
                destruct(goal.x, y);
            }
        }
    }

    void destruct(int x, int y) {
        int current_power_sum = 0;
        int power = 1;
        while (!is_broken[x][y]) {
            Response result = query(x, y, power);
            if (result == Response::finish) {
                cerr << "total_cost=" << total_cost << endl;
                exit(0);
            } else if (result == Response::invalid) {
                cerr << "invalid: y=" << y << " x=" << x << endl;
                exit(1);
            }
            current_power_sum += power;
            power = min(power * 2, 5000 - current_power_sum);
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
    show(N, W, K, C, source_pos, house_pos);

    // solver
    Solver solver(N, W, K, C, source_pos, house_pos);
    solver.solve();
    return 0;
}