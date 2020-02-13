#ifndef ENGINE_H
#define ENGINE_H

#include "boardstate.h"
#include "action.h"
#include <vector>
#include <queue>
#include <set>
#include <iostream>

using namespace std;

class Engine {
  private:

    BoardState state;
    vector<Action> hist;


  public:
    Engine() {
      state.turn = 0;
      state.winner = -1.0;
      state.captured_w = 0;
      state.captured_b = 0;
      for (int i=0; i < 19; ++i) {
        for (int j=0; j < 19; ++j) {
          state.board[i][j] = Empty;
          state.liberties[i][j] = 4;
          if (i == 0 || i == 18)
            state.liberties[i][j] -= 1;
          if (j == 0 || j == 18)
            state.liberties[i][j] -= 1;
        }
      }
    }

    BoardState getBoardState() {
      return state;
    }

    int groupLiberties(BoardState BS, Action a) {
      queue<Action> Queue;
      set<Action> Liberties;

      bool visited[19][19] = {0};

      piece friendly = BS.board[a.i][a.j];
      if (friendly == Empty)
        return -1;

      //flood fill algorithm
      Queue.push(a);
      while (!Queue.empty()) {
        Action c = Queue.front();
        Queue.pop();
        visited[c.i][c.j] = true;
        if (BS.board[c.i][c.j] == friendly) {
          if (c.i+1 < 19 && !visited[c.i+1][c.j]) {
            Action n = {c.i+1, c.j};
            Queue.push(n);
          }
          if (c.j+1 < 19 && !visited[c.i][c.j+1]) {
            Action n = {c.i, c.j+1};
            Queue.push(n);
          }
          if (c.i-1 >= 0 && !visited[c.i-1][c.j]) {
            Action n = {c.i-1, c.j};
            Queue.push(n);
          }
          if (c.j-1 >= 0 && !visited[c.i][c.j-1]) {
            Action n = {c.i, c.j-1};
            Queue.push(n);
          }
        }
        else if (BS.board[c.i][c.j] == Empty) {
          Action n = {c.i, c.j};
          Liberties.insert(n);
        }
      }
      return Liberties.size();
    }

    BoardState captureDeadStones(BoardState BS, Action a) {
      BoardState S = BS;
      queue<Action> Queue;
      vector<Action> Group;
      set<Action> Liberties;

      bool visited[19][19] = {0};

      piece opponent = S.board[a.i][a.j];
      //if (opponent == Empty) {
      //  return -1;

      //flood fill algorithm
      Queue.push(a);
      while (!Queue.empty()) {
        Action c = Queue.front();
        Queue.pop();
        visited[c.i][c.j] = true;
        if (S.board[c.i][c.j] == opponent) {
          Group.push_back(c);
          if (c.i+1 < 19 && !visited[c.i+1][c.j]) {
            Action n = {c.i+1, c.j};
            Queue.push(n);
          }
          if (c.j+1 < 19 && !visited[c.i][c.j+1]) {
            Action n = {c.i, c.j+1};
            Queue.push(n);
          }
          if (c.i-1 >= 0 && !visited[c.i-1][c.j]) {
            Action n = {c.i-1, c.j};
            Queue.push(n);
          }
          if (c.j-1 >= 0 && !visited[c.i][c.j-1]) {
            Action n = {c.i, c.j-1};
            Queue.push(n);
          }
        }
        else if (S.board[c.i][c.j] == Empty) {
          Action n = {c.i, c.j};
          Liberties.insert(n);
        }
      }

      if (Liberties.size() == 0) { // group is captured
        // remove dead stones
        for (Action a : Group) {
          S.board[a.i][a.j] = Empty;
          if (opponent == Black)
            S.captured_b++;
          else
            S.captured_w++;
        }
        for (int i=0; i < 19; ++i) {
          for (int j=0; j < 19; ++j) {
            if (visited[i][j]) {
              // recheck liberties
              int libs = 0;
              if (i+1 < 19 && S.board[i+1][j] == Empty)
                libs++;
              if (j+1 < 19 && S.board[i][j+1] == Empty)
                libs++;
              if (i-1 < 19 && S.board[i-1][j] == Empty)
                libs++;
              if (j-1 < 19 && S.board[i][j-1] == Empty)
                libs++;
              S.liberties[i][j] = libs;
            }
          }
        }
        if (Group.size() == 1) //mark as potential ko
          S.liberties[Group[0].i][Group[0].j] = -2;
      }

      return S;
    }

    int getLegalMoves(vector<Action>* a_list) {
      return getLegalMoves(a_list, state);
    }

    int getLegalMoves(vector<Action>* a_list, BoardState BS) {
      int moves = 0;
      a_list->clear();
      if (BS.winner != -1)
        return moves;

      piece friendly = Black;
      //piece opponent = White;
      if (BS.turn%2 == 1) {
        friendly = White;
        //opponent = Black;
      }
      for (int i=0; i < 8; ++i) {
        for (int j=0; j < 8; ++j) {
          if (BS.board[i][j] == Empty && BS.liberties[i][j] > 0) {
            Action a = {i,j};
            a_list->push_back(a);
          }
          else if (BS.board[i][j] == Empty && BS.liberties[i][j] == 0) {
            // move must capture or else join living group
            if (BS.liberties[i][j] != -2) { // check for ko
              Action Adjacent[4] = {{i+1,j},{i,j+1},{i-1,j},{i,j-1}};
              for (int act=0; act < 4; ++act) {
                if (Adjacent[act].i >= 0 && Adjacent[act].i < 19 &&
                    Adjacent[act].j >= 0 && Adjacent[act].j < 19) {
                  // check if it captures
                  if (BS.board[Adjacent[act].j][Adjacent[act].j] != friendly) {
                    int libs = groupLiberties(BS, Adjacent[act]);
                    if (libs > 1) {
                      Action a = Adjacent[act];
                      a_list->push_back(a);
                      break;
                    }
                  }
                  // else check if any adjacent friendly group is otherwise alive
                  else {
                    int libs = groupLiberties(BS, Adjacent[act]);
                    if (libs <= 1) {
                      Action a = Adjacent[act];
                      a_list->push_back(a);
                      break;
                    }
                  }
                }
              }
            }
          }
        }
      }

      return moves;
    }

    int getScore(BoardState BS) {
      // Chinese scoring:
      // 1 point per stone on board
      // 1 point per prisoner captured
      // 1 point per territory surrounded
      // komi implied with +0.5 to white
      int score = BS.komi;
      // stones on board
      bool visited[19][19] = {false};
      for (int i=0; i < 19; ++i) {
        for (int j=0; j < 19; ++j) {
          if (BS.board[i][j] != Empty) {
            score += BS.board[i][j];
            visited[i][j] = true;
          }
        }
      }
      // territory surrounded
      for (int i=0; i < 19; ++i) {
        for (int j=0; j < 19; ++j) {
          if (!visited[i][j]) {
            // flood fill and check borders
            int points = 0;
            bool w_neighbor = false;
            bool b_neighbor = false;
            queue<Action> Queue;
            Queue.push((Action){i,j});
            while (!Queue.empty()) {
              Action c = Queue.front();
              Queue.pop();
              if (!visited[c.i][c.j]) {
                // add a point and push neighbors to queue
                points++;
                visited[c.i][c.j] = true;
                if (c.i+1 < 19) {
                  if (!visited[c.i+1][c.j]) {
                    Action n = {c.i+1, c.j};
                    Queue.push(n);
                  }
                  else {
                    w_neighbor = w_neighbor || BS.board[c.i][c.j] == White;
                    b_neighbor = b_neighbor || BS.board[c.i][c.j] == Black;
                  }
                }
                if (c.j+1 < 19) {
                  if (!visited[c.i][c.j+1]) {
                    Action n = {c.i, c.j+1};
                    Queue.push(n);
                  }
                  else {
                    w_neighbor = w_neighbor || BS.board[c.i][c.j] == White;
                    b_neighbor = b_neighbor || BS.board[c.i][c.j] == Black;
                  }
                }
                if (c.i-1 >= 0) {
                  if (!visited[c.i-1][c.j]) {
                    Action n = {c.i-1, c.j};
                    Queue.push(n);
                  }
                  else {
                    w_neighbor = w_neighbor || BS.board[c.i][c.j] == White;
                    b_neighbor = b_neighbor || BS.board[c.i][c.j] == Black;
                  }
                }
                if (c.j-1 >= 0) {
                  if (!visited[c.i][c.j-1]) {
                    Action n = {c.i, c.j-1};
                    Queue.push(n);
                  }
                  else {
                    w_neighbor = w_neighbor || BS.board[c.i][c.j] == White;
                    b_neighbor = b_neighbor || BS.board[c.i][c.j] == Black;
                  }
                }
              }
            }
            if (w_neighbor && !b_neighbor)
              score += points;
            else if (!w_neighbor && b_neighbor)
              score -= points;
          }
        }
      }
      // prisoners captured
      score += BS.captured_b;
      score -= BS.captured_w;
      return score;
    }

    void advance(Action a) {
      hist.push_back(a);
      state = advance(state, a);
    }

    BoardState advance(BoardState BS, Action a) {
      // assume move legality check has already been done
      BoardState S = BS;
      int x = a.i;
      int y = a.j;

      // update any previous Ko
      for (int i=0; i < 19; ++i) {
        for (int j=0; j < 19; ++j) {
          if (S.liberties[i][j] == -2)
            S.liberties[i][j] = 0;
        }
      }

      // add stone
      if (S.turn%2 == 0)
        S.board[x][y] = Black;
      else
        S.board[x][y] = White;

      // check for captures
      if (x+1 < 19)
        S.liberties[x+1][y] -= 1;
      if (x-1 >= 0)
        S.liberties[x-1][y] -= 1;
      if (y+1 < 19)
        S.liberties[x][y+1] -= 1;
      if (y-1 >= 0)
        S.liberties[x][y-1] -= 1;

      //piece friendly = Black;
      piece opponent = White;
      if (S.turn%2 == 1) {
        //friendly = White;
        opponent = Black;
      }
      // if any adjacent stone is opponent's color and surrounded, flood fill and mark as dead if no liberties are collected
      Action Adjacent[4] = {{x+1,y},{x,y+1},{x-1,y},{x,y-1}};
      for (int act=0; act < 4; ++act) {
        int i = Adjacent[act].i;
        int j = Adjacent[act].j;
        if (i >= 0 && i < 19 &&
            j >= 0 && j < 19) {
          if (S.board[i][j] == opponent) {
            S = captureDeadStones(S, Adjacent[act]);
          }
        }
      }
      // if only 1 stone was captured and move played now has 1 liberty and is surrounded, mark as ko
      if (S.liberties[x][y] == 1 && (S.captured_w-BS.captured_w == 1 || S.captured_b-BS.captured_b == 1)) {
        bool is_ko = true;
        Action ko = {-1,-1};
        for (int act=0; act < 4; ++act) {
          int i = Adjacent[act].i;
          int j = Adjacent[act].j;
          if (i >= 0 && i < 19 &&
              j >= 0 && j < 19) {
            if (S.board[i][j] == Empty &&
                S.liberties[i][j] == -2) {
              ko.i = i; ko.j = j;
            }
            else if (S.board[i][j] != opponent)
              is_ko = false;
          }
        }
        if (!is_ko && ko.i != -1 && ko.j != -1)
          S.liberties[ko.i][ko.j] = 0;
      }
      else {
        for (int act=0; act < 4; ++act) {
          int i = Adjacent[act].i;
          int j = Adjacent[act].j;
          if (i >= 0 && i < 19 &&
              j >= 0 && j < 19) {
            if (S.liberties[i][j] == -2) {
              S.liberties[i][j] = 0;
            }
          }
        }
      }

      S.turn++;
      return S;
    }

    vector<Action> getHist() {
      return hist;
    }

    void printBoard() {
      printBoard(state);
    }

    void printBoard(BoardState BS) {
      //string wh = "\xe2\x9a\xab";
      //string bl = "\xe2\x9a\xaa";
      string wh = "x";
      string bl = "o";

      cout << "Prisoners\t" << wh << " " << BS.captured_w << "\t" << bl << " " << BS.captured_b << endl;
      cout << "   A B C D E F G H I J K L M N O P Q R S\n";
      for (int j = 0; j < 19; ++j) {
        cout << j+1 << ((j<9)?" ":"");
        for (int i = 0; i < 19; ++i) {
          switch (BS.board[i][j]) {
            case White:
              cout << " " << wh;
              break;
            case Black:
              cout << " " << bl;
              break;
            default: // blank
              cout << " +";
              break;
          }
        }
        cout << " " << j+1 << endl;
      }
      cout << "   A B C D E F G H I J K L M N O P Q R S\n";
    }

};

#endif
