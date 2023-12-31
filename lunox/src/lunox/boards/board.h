#ifndef LNX_BOARD_H
#define LNX_BOARD_H

#include "lunox/core/core.h"

#define LNX_BOARD_WIDTH 8
#define LNX_BOARD_HEIGHT 8
#define LNX_BOARD_SIZE (LNX_BOARD_WIDTH * LNX_BOARD_HEIGHT)

#define LNX_SQUARE_TO_FILE(square) ((square) % LNX_BOARD_WIDTH)
#define LNX_SQUARE_TO_RANK(square) ((square) / LNX_BOARD_WIDTH)
#define LNX_FILE_RANK_TO_SQUARE(file, rank) (LNX_BOARD_WIDTH * (rank) + (file))

enum
{
    LNX_SQUARE_A1 = 0, LNX_SQUARE_B1, LNX_SQUARE_C1, LNX_SQUARE_D1, LNX_SQUARE_E1, LNX_SQUARE_F1, LNX_SQUARE_G1, LNX_SQUARE_H1,
    LNX_SQUARE_A2, LNX_SQUARE_B2, LNX_SQUARE_C2, LNX_SQUARE_D2, LNX_SQUARE_E2, LNX_SQUARE_F2, LNX_SQUARE_G2, LNX_SQUARE_H2,
    LNX_SQUARE_A3, LNX_SQUARE_B3, LNX_SQUARE_C3, LNX_SQUARE_D3, LNX_SQUARE_E3, LNX_SQUARE_F3, LNX_SQUARE_G3, LNX_SQUARE_H3,
    LNX_SQUARE_A4, LNX_SQUARE_B4, LNX_SQUARE_C4, LNX_SQUARE_D4, LNX_SQUARE_E4, LNX_SQUARE_F4, LNX_SQUARE_G4, LNX_SQUARE_H4,
    LNX_SQUARE_A5, LNX_SQUARE_B5, LNX_SQUARE_C5, LNX_SQUARE_D5, LNX_SQUARE_E5, LNX_SQUARE_F5, LNX_SQUARE_G5, LNX_SQUARE_H5,
    LNX_SQUARE_A6, LNX_SQUARE_B6, LNX_SQUARE_C6, LNX_SQUARE_D6, LNX_SQUARE_E6, LNX_SQUARE_F6, LNX_SQUARE_G6, LNX_SQUARE_H6,
    LNX_SQUARE_A7, LNX_SQUARE_B7, LNX_SQUARE_C7, LNX_SQUARE_D7, LNX_SQUARE_E7, LNX_SQUARE_F7, LNX_SQUARE_G7, LNX_SQUARE_H7,
    LNX_SQUARE_A8, LNX_SQUARE_B8, LNX_SQUARE_C8, LNX_SQUARE_D8, LNX_SQUARE_E8, LNX_SQUARE_F8, LNX_SQUARE_G8, LNX_SQUARE_H8,

    LNX_SQUARE_OFFBOARD
};

typedef uint8_t Square;

enum
{
    LNX_FILE_A = 0,
    LNX_FILE_B,
    LNX_FILE_C,
    LNX_FILE_D,
    LNX_FILE_E,
    LNX_FILE_F,
    LNX_FILE_G,
    LNX_FILE_H
};

typedef uint8_t File;

enum
{
    LNX_RANK_1 = 0,
    LNX_RANK_2,
    LNX_RANK_3,
    LNX_RANK_4,
    LNX_RANK_5,
    LNX_RANK_6,
    LNX_RANK_7,
    LNX_RANK_8
};

typedef uint8_t Rank;

#endif