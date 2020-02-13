#include <cstdlib>
#include <iostream>
#include "engine.h"

using namespace std;

int main() {
  //FUNDAMENTALS
  //should instantiate
  Engine* E = new Engine();
  if (E != NULL)
    cout << "+ Instantiates\n";
  else
    cout << "- Failed to instantiate\n";

  //should return board state
  BoardState S = E->getBoardState();
  bool success = true;
  for (int i=0; i < 19; ++i) {
    for (int j=0; j < 19; ++j) {
      // Check board state //
      if (S.board[i][j] != 0)
        success = false;
    }
  }
  if (success)
    cout << "+ returned valid boardstate\n";
  else
    cout << "- returned invalid boardstate\n";

  //should retun list of legal moves
  vector<Action> movelist;
  unsigned int num_moves = E->getLegalMoves(&movelist, S);
    //TODO: check that the moves are valid)
  if (movelist.size() == num_moves && num_moves == 19*19)
    cout << "+ returned valid movelist\n";
  else {
    cout << "- returned invalid movelist\nmoves: " << num_moves << " " << movelist.size() << endl;
    for (Action m : movelist)
      cout << m.i << "," << m.j << endl;
  }

  //should make moves and update board
  Action a = {4,4};
  Action b = {4,3};
  Action c = {3,4};
  Action d = {3,3};
  E->advance(a);
  E->advance(b);
  E->advance(c);
  E->advance(d);
  E->printBoard();
  cout << E->getHist().size() << endl;

  //TODO:
  //should detect game end states
  // test that moves capture pieces



}
