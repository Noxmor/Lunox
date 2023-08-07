#include "bitboard.h"

#include <stdio.h>

Bitboard white_pawn_pushs[LNX_BOARD_WIDTH * LNX_BOARD_HEIGHT];
Bitboard black_pawn_pushs[LNX_BOARD_WIDTH * LNX_BOARD_HEIGHT];
Bitboard white_pawn_attacks[LNX_BOARD_WIDTH * LNX_BOARD_HEIGHT];
Bitboard black_pawn_attacks[LNX_BOARD_WIDTH * LNX_BOARD_HEIGHT];
Bitboard knight_attacks[LNX_BOARD_WIDTH * LNX_BOARD_HEIGHT];
Bitboard king_attacks[LNX_BOARD_WIDTH * LNX_BOARD_HEIGHT];

Bitboard bishop_blocker_masks[LNX_BOARD_WIDTH * LNX_BOARD_HEIGHT];
Bitboard rook_blocker_masks[LNX_BOARD_WIDTH * LNX_BOARD_HEIGHT];

static void init_white_pawn_push_bitboard(Square square)
{
    Bitboard pawn_bitboard = LNX_BIT(square);
    white_pawn_pushs[square] = LNX_BITBOARD_EMPTY;

    white_pawn_pushs[square] |= (pawn_bitboard << LNX_BOARD_WIDTH) & LNX_BITBOARD_NOT_RANK_1;

    if(LNX_SQUARE_TO_RANK(square) == LNX_RANK_2)
        white_pawn_pushs[square] |= pawn_bitboard << 2 * LNX_BOARD_WIDTH;
}

static void init_black_pawn_push_bitboard(Square square)
{
    Bitboard pawn_bitboard = LNX_BIT(square);
    black_pawn_pushs[square] = LNX_BITBOARD_EMPTY;

    black_pawn_pushs[square] |= (pawn_bitboard >> LNX_BOARD_WIDTH) & LNX_BITBOARD_NOT_RANK_8;

    if(LNX_SQUARE_TO_RANK(square) == LNX_RANK_7)
        black_pawn_pushs[square] |= pawn_bitboard >> 2 * LNX_BOARD_WIDTH;
}

static void init_white_pawn_attack_bitboard(Square square)
{
    Bitboard pawn_bitboard = LNX_BIT(square);
    white_pawn_attacks[square] = LNX_BITBOARD_EMPTY;

    white_pawn_attacks[square] |= (pawn_bitboard << (LNX_BOARD_WIDTH - 1)) & LNX_BITBOARD_NOT_FILE_H;
    white_pawn_attacks[square] |= (pawn_bitboard << (LNX_BOARD_WIDTH + 1)) & LNX_BITBOARD_NOT_FILE_A;
}

static void init_black_pawn_attack_bitboard(Square square)
{
    Bitboard pawn_bitboard = LNX_BIT(square);
    black_pawn_attacks[square] = LNX_BITBOARD_EMPTY;

    black_pawn_attacks[square] |= (pawn_bitboard >> (LNX_BOARD_WIDTH + 1)) & LNX_BITBOARD_NOT_FILE_H;
    black_pawn_attacks[square] |= (pawn_bitboard >> (LNX_BOARD_WIDTH - 1)) & LNX_BITBOARD_NOT_FILE_A;
}

static void init_knight_attack_bitboard(Square square)
{
    Bitboard knight_bitboard = LNX_BIT(square);
    knight_attacks[square] = LNX_BITBOARD_EMPTY;

    knight_attacks[square] |= ((knight_bitboard >> 2) << LNX_BOARD_WIDTH) & ~(LNX_BITBOARD_NOT_FILE_H ^ LNX_BITBOARD_NOT_FILE_G);
    knight_attacks[square] |= ((knight_bitboard >> 2) >> LNX_BOARD_WIDTH) & ~(LNX_BITBOARD_NOT_FILE_H ^ LNX_BITBOARD_NOT_FILE_G);

    knight_attacks[square] |= ((knight_bitboard << 2) << LNX_BOARD_WIDTH) & ~(LNX_BITBOARD_NOT_FILE_A ^ LNX_BITBOARD_NOT_FILE_B);
    knight_attacks[square] |= ((knight_bitboard << 2) >> LNX_BOARD_WIDTH) & ~(LNX_BITBOARD_NOT_FILE_A ^ LNX_BITBOARD_NOT_FILE_B);

    knight_attacks[square] |= ((knight_bitboard << (2 * LNX_BOARD_WIDTH)) >> 1) & LNX_BITBOARD_NOT_FILE_H;
    knight_attacks[square] |= ((knight_bitboard << (2 * LNX_BOARD_WIDTH)) << 1) & LNX_BITBOARD_NOT_FILE_A;

    knight_attacks[square] |= ((knight_bitboard >> (2 * LNX_BOARD_WIDTH)) >> 1) & LNX_BITBOARD_NOT_FILE_H;
    knight_attacks[square] |= ((knight_bitboard >> (2 * LNX_BOARD_WIDTH)) << 1) & LNX_BITBOARD_NOT_FILE_A;
}

static void init_king_attack_bitboard(Square square)
{
    Bitboard king_bitboard = LNX_BIT(square);
    king_attacks[square] = LNX_BITBOARD_EMPTY;

    king_attacks[square] |= (king_bitboard << (LNX_BOARD_HEIGHT - 1)) & LNX_BITBOARD_NOT_FILE_H;
    king_attacks[square] |= (king_bitboard >> 1) & LNX_BITBOARD_NOT_FILE_H;
    king_attacks[square] |= (king_bitboard >> (LNX_BOARD_HEIGHT + 1)) & LNX_BITBOARD_NOT_FILE_H;

    king_attacks[square] |= king_bitboard << LNX_BOARD_HEIGHT;
    king_attacks[square] |= king_bitboard >> LNX_BOARD_HEIGHT;

    king_attacks[square] |= (king_bitboard << (LNX_BOARD_HEIGHT + 1)) & LNX_BITBOARD_NOT_FILE_A;
    king_attacks[square] |= (king_bitboard << 1) & LNX_BITBOARD_NOT_FILE_A;
    king_attacks[square] |= (king_bitboard >> (LNX_BOARD_HEIGHT - 1)) & LNX_BITBOARD_NOT_FILE_A;
}

static void init_bishop_blocker_mask(Square square)
{
    Bitboard bishop_bitboard = BIT(square);
    bishop_blocker_masks[square] = LNX_BITBOARD_EMPTY;

    const File file = LNX_SQUARE_TO_FILE(square);
    const Rank rank = LNX_SQUARE_TO_RANK(square);

    uint8_t y = rank;
    uint8_t x = file;
    while(x < LNX_BOARD_WIDTH - 1 && y < LNX_BOARD_HEIGHT - 1)
        bishop_blocker_masks[square] |= BIT(LNX_FILE_RANK_TO_SQUARE(x++, y++));

    y = rank;
    x = file;
    while(x < LNX_BOARD_WIDTH - 1 && y)
        bishop_blocker_masks[square] |= BIT(LNX_FILE_RANK_TO_SQUARE(x++, y--));

    y = rank;
    x = file;
    while(x && y)
        bishop_blocker_masks[square] |= BIT(LNX_FILE_RANK_TO_SQUARE(x--, y--));

    y = rank;
    x = file;
    while(x && y < LNX_BOARD_HEIGHT - 1)
        bishop_blocker_masks[square] |= BIT(LNX_FILE_RANK_TO_SQUARE(x--, y++));

    bishop_blocker_masks[square] &= ~bishop_bitboard;
}

static void init_rook_blocker_mask(Square square)
{
    Bitboard rook_bitboard = BIT(square);
    rook_blocker_masks[square] = LNX_BITBOARD_EMPTY;

    const File file = LNX_SQUARE_TO_FILE(square);
    const Rank rank = LNX_SQUARE_TO_RANK(square);

    for(uint8_t i = 0; i < LNX_BOARD_WIDTH; ++i)
    {
        rook_blocker_masks[square] |= BIT(LNX_FILE_RANK_TO_SQUARE((file + i) % LNX_BOARD_WIDTH, rank));
        rook_blocker_masks[square] |= BIT(LNX_FILE_RANK_TO_SQUARE(file, (rank + i) % LNX_BOARD_HEIGHT));
    }

    rook_blocker_masks[square] &= ~(BIT(LNX_FILE_RANK_TO_SQUARE(file, LNX_RANK_1)) | BIT(LNX_FILE_RANK_TO_SQUARE(file, LNX_RANK_8)) | BIT(LNX_FILE_RANK_TO_SQUARE(LNX_FILE_A, rank)) | BIT(LNX_FILE_RANK_TO_SQUARE(LNX_FILE_H, rank)));

    rook_blocker_masks[square] &= ~rook_bitboard;
}

void bitboard_init(void)
{
    for(Square square = LNX_SQUARE_A1; square <= LNX_SQUARE_H8; ++square)
    {
        init_white_pawn_push_bitboard(square);
        init_black_pawn_push_bitboard(square);
        init_white_pawn_attack_bitboard(square);
        init_black_pawn_attack_bitboard(square);
        init_knight_attack_bitboard(square);
        init_king_attack_bitboard(square);

        init_bishop_blocker_mask(square);
        init_rook_blocker_mask(square);
    }
}

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