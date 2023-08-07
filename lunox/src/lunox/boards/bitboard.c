#include "bitboard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Bitboard white_pawn_pushs[LNX_BOARD_SIZE];
Bitboard black_pawn_pushs[LNX_BOARD_SIZE];
Bitboard white_pawn_attacks[LNX_BOARD_SIZE];
Bitboard black_pawn_attacks[LNX_BOARD_SIZE];
Bitboard knight_attacks[LNX_BOARD_SIZE];
Bitboard king_attacks[LNX_BOARD_SIZE];

Bitboard bishop_blocker_masks[LNX_BOARD_SIZE];
Bitboard rook_blocker_masks[LNX_BOARD_SIZE];

Bitboard bishop_blocker_shifts[LNX_BOARD_SIZE];
Bitboard rook_blocker_shifts[LNX_BOARD_SIZE];

Bitboard* bishop_blocker_boards[LNX_BOARD_SIZE];
Bitboard* rook_blocker_boards[LNX_BOARD_SIZE];

Bitboard* bishop_attacks[LNX_BOARD_SIZE];
Bitboard* rook_attacks[LNX_BOARD_SIZE];

uint64_t bishop_magics[LNX_BOARD_SIZE];
uint64_t rook_magics[LNX_BOARD_SIZE];

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
    Bitboard bishop_bitboard = LNX_BIT(square);
    bishop_blocker_masks[square] = LNX_BITBOARD_EMPTY;

    File file = LNX_SQUARE_TO_FILE(square);
    Rank rank = LNX_SQUARE_TO_RANK(square);

    uint8_t y = rank;
    uint8_t x = file;
    while(x < LNX_BOARD_WIDTH - 1 && y < LNX_BOARD_HEIGHT - 1)
        bishop_blocker_masks[square] |= LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x++, y++));

    y = rank;
    x = file;
    while(x < LNX_BOARD_WIDTH - 1 && y)
        bishop_blocker_masks[square] |= LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x++, y--));

    y = rank;
    x = file;
    while(x && y)
        bishop_blocker_masks[square] |= LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x--, y--));

    y = rank;
    x = file;
    while(x && y < LNX_BOARD_HEIGHT - 1)
        bishop_blocker_masks[square] |= LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x--, y++));

    bishop_blocker_masks[square] &= ~bishop_bitboard;
}

static void init_bishop_blocker_shift(Square square)
{
    bishop_blocker_shifts[square] = LNX_BOARD_SIZE - LNX_BIT_COUNT(bishop_blocker_masks[square]);
}

static void init_rook_blocker_mask(Square square)
{
    Bitboard rook_bitboard = LNX_BIT(square);
    rook_blocker_masks[square] = LNX_BITBOARD_EMPTY;

    File file = LNX_SQUARE_TO_FILE(square);
    Rank rank = LNX_SQUARE_TO_RANK(square);

    for(uint8_t i = 0; i < LNX_BOARD_WIDTH; ++i)
    {
        rook_blocker_masks[square] |= LNX_BIT(LNX_FILE_RANK_TO_SQUARE((file + i) % LNX_BOARD_WIDTH, rank));
        rook_blocker_masks[square] |= LNX_BIT(LNX_FILE_RANK_TO_SQUARE(file, (rank + i) % LNX_BOARD_HEIGHT));
    }

    rook_blocker_masks[square] &= ~(LNX_BIT(LNX_FILE_RANK_TO_SQUARE(file, LNX_RANK_1)) | LNX_BIT(LNX_FILE_RANK_TO_SQUARE(file, LNX_RANK_8)) | LNX_BIT(LNX_FILE_RANK_TO_SQUARE(LNX_FILE_A, rank)) | LNX_BIT(LNX_FILE_RANK_TO_SQUARE(LNX_FILE_H, rank)));

    rook_blocker_masks[square] &= ~rook_bitboard;
}

static void init_rook_blocker_shift(Square square)
{
    rook_blocker_shifts[square] = LNX_BOARD_SIZE - LNX_BIT_COUNT(rook_blocker_masks[square]);
}

static Bitboard calculate_blocker_permutation(uint64_t iteration, Bitboard blocker_mask)
{
    Bitboard blocker_permutation = LNX_BITBOARD_EMPTY;

    while (iteration)
    {
        if (iteration & 1)
        {
            uint8_t shift = LNX_BIT_LSB_INDEX(blocker_mask);
            blocker_permutation |= (1ull << shift);
        }

        iteration = iteration >> 1;
        blocker_mask &= (blocker_mask - 1);
    }

    return blocker_permutation;
}

static void init_temp_bishop_attack_bitboard(Square square, uint64_t index)
{
    Bitboard blocker_board = bishop_blocker_boards[square][index];

    File file = LNX_SQUARE_TO_FILE(square);
    Rank rank = LNX_SQUARE_TO_RANK(square);

    int8_t x;
    int8_t y;

    for(x = file + 1, y = rank + 1; x < LNX_BOARD_WIDTH && y < LNX_BOARD_HEIGHT; ++x, ++y)
    {
        bishop_attacks[square][index] |= LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x, y));

        if (blocker_board & LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x, y)))
            break;
    }

    for(x = file + 1, y = rank - 1; x < LNX_BOARD_WIDTH && y >= 0; ++x, --y)
    {
        bishop_attacks[square][index] |= LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x, y));

        if (blocker_board & LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x, y)))
            break;
    }

    for(x = file - 1, y = rank - 1; x >= 0 && y >= 0; --x, --y)
    {
        bishop_attacks[square][index] |= LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x, y));

        if (blocker_board & LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x, y)))
            break;
    }

    for(x = file - 1, y = rank + 1; x >= 0 && y < LNX_BOARD_HEIGHT; --x, ++y)
    {
        bishop_attacks[square][index] |= LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x, y));

        if (blocker_board & LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x, y)))
            break;
    }
}

static LunoxBool init_bishop_attack_board(Square square)
{
    uint8_t bits = LNX_BIT_COUNT(bishop_blocker_masks[square]);
    uint64_t permutations = 1ull << bits;

    bishop_blocker_boards[square] = malloc(permutations * sizeof(Bitboard));
    bishop_attacks[square] = calloc(permutations, sizeof(Bitboard));

    for(uint64_t i = 0; i < permutations; ++i)
    {
        bishop_blocker_boards[square][i] = calculate_blocker_permutation(i, bishop_blocker_masks[square]);
        init_temp_bishop_attack_bitboard(square, i);
    }

    Bitboard used_attacks[permutations];
    memset(used_attacks, LNX_BITBOARD_EMPTY, sizeof(used_attacks));

    LunoxBool success = LNX_TRUE;
    for(uint64_t i = 0; i < permutations; ++i)
    {
        uint64_t magic_index = ((bishop_blocker_boards[square][i] * bishop_magics[square]) >> bishop_blocker_shifts[square]);

        if(used_attacks[magic_index] == LNX_BITBOARD_EMPTY)
            used_attacks[magic_index] = bishop_attacks[square][i];
        else if(used_attacks[magic_index] != bishop_attacks[square][i])
        {
            success = LNX_FALSE;
            break;
        }
    }

    memcpy(bishop_attacks[square], used_attacks, permutations * sizeof(Bitboard));

    return success;
}

static void init_temp_rook_attack_bitboard(Square square, uint64_t index)
{
    Bitboard blocker_board = rook_blocker_boards[square][index];

    File file = LNX_SQUARE_TO_FILE(square);
    Rank rank = LNX_SQUARE_TO_RANK(square);

    for(uint8_t x = file + 1; x < LNX_BOARD_WIDTH; ++x)
    {
        rook_attacks[square][index] |= LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x, rank));

        if (blocker_board & LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x, rank)))
            break;
    }

    for(int8_t x = file - 1; x >= 0; --x)
    {
        rook_attacks[square][index] |= LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x, rank));

        if (blocker_board & LNX_BIT(LNX_FILE_RANK_TO_SQUARE(x, rank)))
            break;
    }

    for(uint8_t y = rank + 1; y < LNX_BOARD_HEIGHT; ++y)
    {
        rook_attacks[square][index] |= LNX_BIT(LNX_FILE_RANK_TO_SQUARE(file, y));

        if (blocker_board & LNX_BIT(LNX_FILE_RANK_TO_SQUARE(file, y)))
            break;
    }

    for(int8_t y = rank - 1; y >= 0; --y)
    {
        rook_attacks[square][index] |= LNX_BIT(LNX_FILE_RANK_TO_SQUARE(file, y));

        if (blocker_board & LNX_BIT(LNX_FILE_RANK_TO_SQUARE(file, y)))
            break;
    }
}

static LunoxBool init_rook_attack_board(Square square)
{
    uint8_t bits = LNX_BIT_COUNT(rook_blocker_masks[square]);
    uint64_t permutations = 1ull << bits;

    rook_blocker_boards[square] = malloc(permutations * sizeof(Bitboard));
    rook_attacks[square] = calloc(permutations, sizeof(Bitboard));

    for(uint64_t i = 0; i < permutations; ++i)
    {
        rook_blocker_boards[square][i] = calculate_blocker_permutation(i, rook_blocker_masks[square]);
        init_temp_rook_attack_bitboard(square, i);
    }

    Bitboard used_attacks[permutations];
    memset(used_attacks, LNX_BITBOARD_EMPTY, sizeof(used_attacks));

    LunoxBool success = LNX_TRUE;
    for(uint64_t i = 0; i < permutations; ++i)
    {
        uint64_t magic_index = ((rook_blocker_boards[square][i] * rook_magics[square]) >> rook_blocker_shifts[square]);

        if(used_attacks[magic_index] == LNX_BITBOARD_EMPTY)
            used_attacks[magic_index] = rook_attacks[square][i];
        else if(used_attacks[magic_index] != rook_attacks[square][i])
        {
            success = LNX_FALSE;
            break;
        }
    }

    memcpy(rook_attacks[square], used_attacks, permutations * sizeof(Bitboard));

    return success;
}

void bitboard_init()
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
        init_bishop_blocker_shift(square);

        init_rook_blocker_mask(square);
        init_rook_blocker_shift(square);
    }
}

LunoxBool bitboard_init_magics(const char* magics_filepath)
{
    LNX_ASSERT(magics_filepath != NULL);

    FILE* f = fopen(magics_filepath, "rb");

    if(f == NULL)
        return LNX_FALSE;

    if(fread(bishop_magics, sizeof(bishop_magics), 1, f) != 1)
    {
        fclose(f);
        return LNX_FALSE;
    }

    if(fread(rook_magics, sizeof(rook_magics), 1, f) != 1)
    {
        fclose(f);
        return LNX_FALSE;
    }

    fclose(f);

    for(Square square = LNX_SQUARE_A1; square <= LNX_SQUARE_H8; ++square)
    {
        if(!init_bishop_attack_board(square))
            return LNX_FALSE;

        if(!init_rook_attack_board(square))
            return LNX_FALSE;
    }

    return LNX_TRUE;
}

void bitboard_init_default_magics(void)
{
    //TODO: Implement
}

Bitboard bitboard_get_bishop_attacks(Square bishop, Bitboard occupancy)
{
    occupancy &= bishop_blocker_masks[bishop];
    uint64_t magic_index = (occupancy * bishop_magics[bishop]) >> bishop_blocker_shifts[bishop];
    return bishop_attacks[bishop][magic_index];
}

Bitboard bitboard_get_rook_attacks(Square rook, Bitboard occupancy)
{
    occupancy &= rook_blocker_masks[rook];
    uint64_t magic_index = (occupancy * rook_magics[rook]) >> rook_blocker_shifts[rook];
    return rook_attacks[rook][magic_index];
}

Bitboard bitboard_get_queen_attacks(Square queen, Bitboard occupancy)
{
    return bitboard_get_bishop_attacks(queen, occupancy) | bitboard_get_rook_attacks(queen, occupancy);
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