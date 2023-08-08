#ifndef LNX_POSITION_H
#define LNX_POSITION_H

#include "lunox/boards/bitboard.h"

#include "lunox/move/move.h"

typedef struct Side
{
    Bitboard occupancy;
    Bitboard pawns;
    Bitboard knights;
    Bitboard bishops;
    Bitboard rooks;
    Bitboard queens;
    Bitboard kings;
} Side;

void side_calculate_occupancy(Side* side);

enum
{
    LNX_CASTLING_PERM_NONE = 0,
    LNX_CASTLING_PERM_WHITE_KINGSIDE = LNX_BIT(0),
    LNX_CASTLING_PERM_WHITE_QUEENSIDE = LNX_BIT(1),
    LNX_CASTLING_PERM_BLACK_KINGSIDE = LNX_BIT(2),
    LNX_CASTLING_PERM_BLACK_QUEENSIDE = LNX_BIT(3),
};

typedef struct Position
{
    Side sides[2];

    Bitboard occupancy;

    uint8_t castling_perms;

    Square ep_square;
    
    uint8_t plys;

    uint8_t side_to_move;
} Position;

void position_parse_fen(Position* pos, const char* fen);

void position_startpos(Position* pos);

void position_calculate_occupancy(Position* pos);

void position_make_move(Position* pos, Move move);

uint8_t position_attack_count_on_square(const Position* pos, uint8_t side, Square square);

LunoxBool position_validate(const Position* pos);

void position_print(const Position* pos, uint8_t side);

#endif