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

typedef struct Position
{
    Side sides[2];

    Bitboard occupancy;

    uint8_t side_to_move;
} Position;

void position_parse_fen(Position* pos, const char* fen);

void position_startpos(Position* pos);

void position_calculate_occupancy(Position* pos);

void position_make_move(Position* pos, Move move);

LunoxBool position_validate(const Position* pos);

void position_print(const Position* pos, uint8_t side);

#endif