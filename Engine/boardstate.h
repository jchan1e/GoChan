#ifndef BOARDSTATE_H
#define BOARDSTATE_H

enum piece : char {
  White = 1,
  Black = -1,
  Empty = 0
};

struct BoardState {
  piece board[19][19];
  char liberties[19][19];
  short unsigned int turn;
  float winner;
  short int komi; //+0.5
  short int points;
  short unsigned int captured_w;
  short unsigned int captured_b;
};

inline bool operator==(const BoardState& lhs, const BoardState& rhs) {
  if (lhs.captured_w != rhs.captured_w)
    return false;
  if (lhs.captured_b != rhs.captured_b)
    return false;
  for (int i=0; i < 19; ++i) {
    for (int j=0; j < 19; ++j) {
      if (lhs.board[i][j] != rhs.board[i][j])
        return false;
    }
  }
  return true;
}

#endif
