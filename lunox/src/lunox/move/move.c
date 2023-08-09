#include "move.h"

#include <lunox/boards/board.h>

char promotion_piece_type_to_char(PromotionPieceType type)
{
    switch(type)
    {
        case LNX_PROMOTION_PIECE_TYPE_KNIGHT: return 'n';
        case LNX_PROMOTION_PIECE_TYPE_BISHOP: return 'b';
        case LNX_PROMOTION_PIECE_TYPE_ROOK: return 'r';
        case LNX_PROMOTION_PIECE_TYPE_QUEEN: return 'q';
    }

    LNX_VERIFY(LNX_FALSE);

    return ' ';
}

void move_to_str(char* dest, Move move)
{
    LNX_ASSERT(dest != NULL);

    LNX_VERIFY(move != LNX_MOVE_NONE);

    Square from = LNX_MOVE_GET_FROM(move);
    Square to = LNX_MOVE_GET_TO(move);

    dest[0] = LNX_SQUARE_TO_FILE(from) + 'a';
    dest[1] = LNX_SQUARE_TO_RANK(from) + '1';
    dest[2] = LNX_SQUARE_TO_FILE(to) + 'a';
    dest[3] = LNX_SQUARE_TO_RANK(to) + '1';

    MoveType type = LNX_MOVE_GET_TYPE(move);
    if(type == LNX_MOVE_TYPE_PROMOTION)
    {
        dest[4] = promotion_piece_type_to_char(LNX_MOVE_GET_PROMOTION_PIECE(move));
        dest[5] = '\0';
    }
    else
        dest[4] = '\0';
}