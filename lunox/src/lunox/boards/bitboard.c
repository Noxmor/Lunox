#include "bitboard.h"

#include <stdio.h>

#include "lunox/boards/board.h"

static void bitboard_print_white(Bitboard bitboard)
{
    for(uint8_t y = 0; y < LNX_BOARD_HEIGHT; ++y)
    {
        printf("  +---+---+---+---+---+---+---+---+\n");

        printf("%c ", '8' - y);

        for(uint8_t x = 0; x < LNX_BOARD_WIDTH; ++x)
            printf("| %c ", (char)('0' + LNX_BIT_GET(bitboard, (LNX_BOARD_HEIGHT - 1 - y) * LNX_BOARD_WIDTH + x)));

        printf("|\n");
    }

    printf("  +---+---+---+---+---+---+---+---+\n");
    printf("    a   b   c   d   e   f   g   h\n");
}

static void bitboard_print_black(Bitboard bitboard)
{
    for(uint8_t y = 0; y < LNX_BOARD_HEIGHT; ++y)
    {
        printf("  +---+---+---+---+---+---+---+---+\n");

        printf("%c ", '1' + y);

        for(uint8_t x = 0; x < LNX_BOARD_WIDTH; ++x)
            printf("| %c ", (char)('0' + LNX_BIT_GET(bitboard, y * LNX_BOARD_WIDTH + (LNX_BOARD_WIDTH - 1 - x))));

        printf("|\n");
    }

    printf("  +---+---+---+---+---+---+---+---+\n");
    printf("    h   g   f   e   d   c   b   a\n");
}

void bitboard_print(Bitboard bitboard, uint8_t side)
{
    switch(side)
    {
        case LNX_SIDE_WHITE: bitboard_print_white(bitboard); break;
        case LNX_SIDE_BLACK: bitboard_print_black(bitboard); break;

        default: LNX_ASSERT(LNX_FALSE);
    }
}