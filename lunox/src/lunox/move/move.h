#ifndef LNX_MOVE_H
#define LNX_MOVE_H

#include "lunox/core/core.h"

enum
{
    LNX_PIECE_NONE = 0,
    LNX_PIECE_WHITE_PAWN,
    LNX_PIECE_WHITE_KNIGHT,
    LNX_PIECE_WHITE_BISHOP,
    LNX_PIECE_WHITE_ROOK,
    LNX_PIECE_WHITE_QUEEN,
    LNX_PIECE_WHITE_KING,

    LNX_PIECE_BLACK_PAWN,
    LNX_PIECE_BLACK_KNIGHT,
    LNX_PIECE_BLACK_BISHOP,
    LNX_PIECE_BLACK_ROOK,
    LNX_PIECE_BLACK_QUEEN,
    LNX_PIECE_BLACK_KING,

    LNX_PIECE_COUNT
};

typedef uint8_t Piece;

enum
{
    LNX_PIECE_TYPE_NONE = 0,

    LNX_PIECE_TYPE_PAWN,
    LNX_PIECE_TYPE_KNIGHT,
    LNX_PIECE_TYPE_BISHOP,
    LNX_PIECE_TYPE_ROOK,
    LNX_PIECE_TYPE_QUEEN,
    LNX_PIECE_TYPE_KING,

    LNX_PIECE_TYPE_COUNT
};

typedef uint8_t PieceType;

enum
{
    LNX_PROMOTION_PIECE_TYPE_KNIGHT = 0,
    LNX_PROMOTION_PIECE_TYPE_BISHOP,
    LNX_PROMOTION_PIECE_TYPE_ROOK,
    LNX_PROMOTION_PIECE_TYPE_QUEEN,
};

typedef uint8_t PromotionPieceType;

char promotion_piece_type_to_char(PromotionPieceType type);

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

#define LNX_MOVE_CLEAR_FROM(move) ((move) &= ~0b111111)
#define LNX_MOVE_CLEAR_TO(move) ((move) &= ~0b111111000000)
#define LNX_MOVE_CLEAR_PROMOTION_PIECE(move) ((move) &= ~0b11000000000000)
#define LNX_MOVE_CLEAR_TYPE(move) ((move) &= ~0b1100000000000000)

void move_to_str(char* dest, Move move);

#endif