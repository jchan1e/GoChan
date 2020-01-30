#include "Engine/engine.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << "USAGE: review <gamefile>.game\n";
    return 0;
  }

  ifstream infile(argv[1]);
  if (!infile.is_open()) {
    cout << "Error: unable to open file " << argv[1] << endl;
    return 0;
  }

  Engine E;
  E.printBoard();

  vector<Action> movelist;
  float winner;
  infile >> winner;

  while (!infile.eof()) {
    int a, b;
    infile >> a >> b;
    Action A = {a,b};
    movelist.push_back(A);
  }
  for (Action A : movelist) {
    usleep(1000000);
    cout << char('A'+A.i) << A.j+1 << endl;
    E.advance(A);
    E.printBoard();
  }

  string W = "Draw";
  if (winner == 1.0)
    W = "White";
  else if (winner == 0.0)
    W = "Black";
  cout << "Winner: " << winner << endl;

  return 0;
}
