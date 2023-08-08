
#ifndef LNX_MOVEGEN_H
#define LNX_MOVEGEN_H

#include "lunox/boards/position.h"

#include "lunox/move/movelist.h"

void movegen_generate_moves(const Position* pos, MoveList* move_list);

#endif