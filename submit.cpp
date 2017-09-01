#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdlib>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <math.h>
#include <memory>
#include <sstream>
#include <stdio.h>
#include <string>
#include <sys/time.h>
#include <vector>

#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define REP(i, n) FOR(i, 0, n)
#define POW(n) ((n) * (n))
#define ALL(a) (a).begin(), (a).end()
#define dump(v) (cerr << #v << ": " << v << endl)

using namespace std;

const double TIME_LIMIT = 9500;
const int MAX_S = 500;
int BEAM_WIDTH = 10;

unsigned long xor128(void) {
  static unsigned long x = 123456789, y = 362436069, z = 521288629,
                       w = 88675123;
  unsigned long t;
  t = (x ^ (x << 11));
  x = y;
  y = z;
  z = w;
  return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}

// double get_time() {
//   unsigned long long a, d;
//   __asm__ volatile("rdtsc" : "=a"(a), "=d"(d));
//   return (d << 32 | a) / 2500000.0;
// }
double gettime() {
  timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}

int S;
int EVAL_LIST[MAX_S][MAX_S];
const int Dir[8][2] = {{1, -2},  {2, -1}, {2, 1},   {1, 2},
                       {-1, +2}, {-2, 1}, {-2, -1}, {-1, -2}};
enum class DIR { D1 = 0, D2, D3, D4, D5, D6, D7, D8, MAX };
enum class POS { X = 0, Y, MAX };

struct Node {
  unsigned char cost[MAX_S][MAX_S]; // x,y
  unsigned char knights[MAX_S][MAX_S];
  int knight_num, eval;
  int seed;

  void init_data() {
    REP(i, S) REP(j, S) cost[i][j] = knights[i][j] = 0;
    knight_num = eval = 0;
  }

  bool isExitKnight(int x, int y) const {
    if (x < 0 || y < 0 || x >= S || y >= S)
      return false;
    return knights[x][y];
  }
  bool put(int x, int y) {
    if (x < 0 || y < 0 || x >= S || y >= S || knights[x][y] == 1)
      return false;

    knights[x][y] = 1;
    knight_num++;
    assert(knight_num <= S * S);

    REP(i, (int)DIR::MAX) {
      int nx = x + Dir[i][static_cast<int>(POS::X)],
          ny = y + Dir[i][static_cast<int>(POS::Y)];
      if (nx < 0 || ny < 0 || nx >= S || ny >= S)
        continue;
      eval -= abs(cost[nx][ny] - EVAL_LIST[nx][ny]);
      cost[nx][ny]++;
      eval += abs(cost[nx][ny] - EVAL_LIST[nx][ny]);
    }
    return true;
  }

  bool remove(int x, int y) {
    if (x < 0 || y < 0 || x >= S || y >= S || knights[x][y] == 0)
      return false;
    knights[x][y] = 0;
    knight_num--;
    assert(knight_num >= 0);

    REP(i, (int)DIR::MAX) {
      int nx = x + Dir[i][static_cast<int>(POS::X)],
          ny = y + Dir[i][static_cast<int>(POS::Y)];
      if (nx < 0 || ny < 0 || nx >= S || ny >= S)
        continue;
      eval -= abs(cost[nx][ny] - EVAL_LIST[nx][ny]);
      cost[nx][ny]--;
      eval += abs(cost[nx][ny] - EVAL_LIST[nx][ny]);
    }
    return true;
  }

  void frip(int x, int y) {
    if (isExitKnight(x, y)) {
      remove(x, y);
    } else {
      put(x, y);
    }
  }

  void evaluate() {
    eval = 0;
    REP(y, S) REP(x, S) { eval += abs(cost[x][y] - EVAL_LIST[x][y]); }
  }

  void swapNeering(int x, int y) {
    if (x < 0 || y < 0 || x >= S || y >= S)
      return;

    int empX[8];
    int empY[8];
    int inX[8];
    int inY[8];
    int emp, in;
    emp = in = 0;

    REP(i, (int)DIR::MAX) {
      int nx = x + Dir[i][static_cast<int>(POS::X)],
          ny = y + Dir[i][static_cast<int>(POS::Y)];
      if (nx < 0 || ny < 0 || nx >= S || ny >= S)
        continue;

      if (knights[nx][ny]) {
        inX[in] = nx;
        inY[in] = ny;
        in++;
      } else {
        empX[emp] = nx;
        empY[emp] = ny;
        emp++;
      }
    }

    if (emp < 2 || in < 2)
      return;
    REP(i, 2) {
      int eemp = xor128() % emp;
      swap(empX[eemp], empX[emp - 1]);
      swap(empY[eemp], empY[emp - 1]);
      emp--;

      int iin = xor128() % in;
      swap(inX[iin], inX[in - 1]);
      swap(inY[iin], inY[in - 1]);
      in--;

      long long pre = eval;
      remove(inX[in], inY[in]);
      put(empX[emp], empY[emp]);
      if (pre < eval) {
        //ここ２つセットでもいいかもしれない
        put(inX[in], inY[in]);
        remove(empX[emp], empY[emp]);
      }
    }
  }

  vector<string> output() const {
    vector<string> ret(S, string(S, '.'));
    for (int i = 0; i < S; ++i)
      for (int j = 0; j < S; ++j)
        if (knights[i][j] == 1)
          ret[i][j] = 'K';
    return ret;
  }

  bool operator<(const Node &other) const { return eval < other.eval; }
};

void randomNode(Node &node) {
  node.init_data();
  node.evaluate();
  REP(y, S) {
    REP(x, S) {
      if (xor128() % 2 == 0)
        node.put(x, y);
    }
  }
}
Node Main;

class KnightsAttacks {
public:
  // board[x,y]
  vector<string> placeKnights(vector<string> board) {
    const double time_start = gettime();
    const double time_use = 9.8;
    const double time_end = time_start + time_use;
    double time_current;

    S = board.size();
    BEAM_WIDTH = max(1, 2002500 / (S * S) / 9);

    // Example Test
    if (S < 50)
      BEAM_WIDTH = 100;

    REP(x, S) REP(y, S) { EVAL_LIST[x][y] = board[x][y] - '0'; }

    Main.init_data();
    Main.evaluate();

    std::vector<Node> que;
    que.reserve(BEAM_WIDTH);

    //取り敢えずランダムでn個
    REP(i, BEAM_WIDTH) {
      Node node = Main;
      randomNode(node);
      node.seed = i;
      que.emplace_back(node);
    }

    int iter = 0;
    while ((time_current = gettime()) < time_end) {
      iter++;

      REP(i, BEAM_WIDTH) {
        Node &node = que[i];

        REP(x, S) REP(y, S) {
          long long pre = node.eval;

          if (iter % 2 == 0) {
            node.frip(x, y);

            if (pre < node.eval) {
              node.frip(x, y);
            }
          } else {
            //ここをスコアが固定してきたらに変える
            node.swapNeering(x, y);
          }
        }
      }

      // dump(iter);
      // REP(i, min(5, (int)que.size())) {
      //   cerr << front[i].seed << ":" << front[i].eval << " ";
      // }
      // cerr << "\n";
    }

    sort(ALL(que));
    dump(iter);
    dump(que[0].eval);
    vector<string> ret = que[0].output();
    return ret;
  }
};
// -------8<------- end of solution submitted to the website -------8<-------

template <class T> void getVector(vector<T> &v) {
  for (int i = 0; i < v.size(); ++i)
    cin >> v[i];
}

int main() {
  KnightsAttacks ka;
  int S;
  cin >> S;
  vector<string> board(S);
  getVector(board);

  vector<string> ret = ka.placeKnights(board);
  cout << ret.size() << endl;
  for (int i = 0; i < (int)ret.size(); ++i)
    cout << ret[i] << endl;
  cout.flush();
}
