#include "movelist.h"

void move_list_add_move(MoveList* move_list, Move move)
{
    LNX_ASSERT(move_list != NULL);
    LNX_ASSERT(move_list->moves_count < LNX_MOVE_LIST_CAPACITY);

    LNX_VERIFY(move != LNX_MOVE_NONE);

    move_list->moves[move_list->moves_count++] = move;
}

void move_list_reset(MoveList* move_list)
{
    LNX_ASSERT(move_list != NULL);

    move_list->moves_count = 0;
}