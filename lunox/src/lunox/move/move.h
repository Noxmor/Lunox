#ifndef LNX_MOVE_H
#define LNX_MOVE_H

#include "lunox/core/core.h"

enum
{
    LNX_PROMOTION_PIECE_TYPE_KNIGHT = 0,
    LNX_PROMOTION_PIECE_TYPE_BISHOP,
    LNX_PROMOTION_PIECE_TYPE_ROOK,
    LNX_PROMOTION_PIECE_TYPE_QUEEN,
};

typedef uint8_t PromotionPieceType;

enum
{
    LNX_MOVE_TYPE_NORMAL = 0,
    LNX_MOVE_TYPE_PROMOTION,
    LNX_MOVE_TYPE_EN_PASSANT,
    LNX_MOVE_TYPE_CASTLING
};

typedef uint8_t MoveType;

/*
* Move format:
* 00000000 00111111 FROM
* 00001111 11000000 TO
* 00110000 00000000 PROMOTION PIECE
* 11000000 00000000 MOVE TYPE (PROMOTION, EN PASSANT, CASTLING)
*/
typedef uint16_t Move;

#define LNX_MOVE_NONE ((Move) 0)

#define LNX_MOVE_GET_FROM(move) ((move) & 0x3F)
#define LNX_MOVE_GET_TO(move) (((move) >> 6) & 0x3F)
#define LNX_MOVE_GET_PROMOTION_PIECE(move) (((move) >> 12) & 0x3)
#define LNX_MOVE_GET_TYPE(move) (((move) >> 14) & 0x3)

#define LNX_MOVE_SET_FROM(move, from) ((move) |= (from))
#define LNX_MOVE_SET_TO(move, to) ((move) |= ((to) << 6))
#define LNX_MOVE_SET_PROMOTION_PIECE(move, promotion_piece) ((move) |= ((promotion_piece) << 12))
#define LNX_MOVE_SET_TYPE(move, type) ((move) |= ((type) << 14))

#endif