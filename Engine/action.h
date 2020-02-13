#ifndef ACTION_H
#define ACTION_H

#include "boardstate.h"

struct Action {
  int i;
  int j;
};

inline bool operator==(const Action& lhs, const Action& rhs) {
  return lhs.i == rhs.i && lhs.j == rhs.j;
}

inline bool operator<(const Action& lhs, const Action& rhs) {
  return lhs.i < rhs.i || (lhs.i == rhs.i && lhs.j < rhs.j);
}

#endif
