#ifndef LNX_MOVE_LIST_H
#define LNX_MOVE_LIST_H

#include "lunox/move/Move.h"

#define LNX_MOVE_LIST_CAPACITY 256

typedef struct MoveList
{
    Move moves[LNX_MOVE_LIST_CAPACITY];
    uint8_t moves_count;
} MoveList;

void move_list_add_move(MoveList* move_list, Move move);

void move_list_reset(MoveList* move_list);

#endif