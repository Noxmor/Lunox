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
    bishop_magics[0] = 4757015075934634504;
    bishop_magics[1] = 1154119036817506310;
    bishop_magics[2] = 220765450784473096;
    bishop_magics[3] = 1155178256517826560;
    bishop_magics[4] = 4919061787128627200;
    bishop_magics[5] = 4620996692557168964;
    bishop_magics[6] = 117657708630638884;
    bishop_magics[7] = 2882588536371022080;
    bishop_magics[8] = 2341943308882018369;
    bishop_magics[9] = 1137745560928512;
    bishop_magics[10] = 15436448071680;
    bishop_magics[11] = 1152926057364520995;
    bishop_magics[12] = 1165319615471947776;
    bishop_magics[13] = 1153207657340878853;
    bishop_magics[14] = 36663249604976640;
    bishop_magics[15] = 73189146323322028;
    bishop_magics[16] = 76579353324049428;
    bishop_magics[17] = 577041603953230352;
    bishop_magics[18] = 163264317347729424;
    bishop_magics[19] = 1132531371090177;
    bishop_magics[20] = 562967159767560;
    bishop_magics[21] = 41095522845458977;
    bishop_magics[22] = 146929976827970560;
    bishop_magics[23] = 9571249798007808;
    bishop_magics[24] = 2260601275815954;
    bishop_magics[25] = 9329356833817616;
    bishop_magics[26] = 6965380873864631298;
    bishop_magics[27] = 4638716412838297736;
    bishop_magics[28] = 49542894442252296;
    bishop_magics[29] = 72567784538376;
    bishop_magics[30] = 651335467480384000;
    bishop_magics[31] = 1154083688399571232;
    bishop_magics[32] = 2256276260397056;
    bishop_magics[33] = 288529516371378432;
    bishop_magics[34] = 2345250657073643584;
    bishop_magics[35] = 288794426691289408;
    bishop_magics[36] = 1143767244210432;
    bishop_magics[37] = 2324420950449258688;
    bishop_magics[38] = 2342436956322007040;
    bishop_magics[39] = 1163110713774966913;
    bishop_magics[40] = 721158708385701888;
    bishop_magics[41] = 950369695943368704;
    bishop_magics[42] = 5188159983191066629;
    bishop_magics[43] = 1208660151931241472;
    bishop_magics[44] = 2823550222467136;
    bishop_magics[45] = 7881883484422660;
    bishop_magics[46] = 4613955702506127488;
    bishop_magics[47] = 38282831293661696;
    bishop_magics[48] = 289361792139132928;
    bishop_magics[49] = 432664456993439752;
    bishop_magics[50] = 4620713326855586946;
    bishop_magics[51] = 2322169103646720;
    bishop_magics[52] = 288230444904873984;
    bishop_magics[53] = 576531576448499712;
    bishop_magics[54] = 6935547966233120771;
    bishop_magics[55] = 1127008044187788;
    bishop_magics[56] = 4613377213928316928;
    bishop_magics[57] = 288244674124071950;
    bishop_magics[58] = 18031999386190848;
    bishop_magics[59] = 6631463715328;
    bishop_magics[60] = 5332543984898277890;
    bishop_magics[61] = 2342172660168983040;
    bishop_magics[62] = 2357636666295040;
    bishop_magics[63] = 289391769820397713;
    
    rook_magics[0] = 108086665936902016;
    rook_magics[1] = 18014467231056192;
    rook_magics[2] = 72076287346229256;
    rook_magics[3] = 7061664007467583552;
    rook_magics[4] = 144132891933417480;
    rook_magics[5] = 1224984596247216200;
    rook_magics[6] = 4863888697080168960;
    rook_magics[7] = 36029346827223296;
    rook_magics[8] = 2379308125313302784;
    rook_magics[9] = 864972741944610944;
    rook_magics[10] = 36310479764783680;
    rook_magics[11] = 2882866788786258184;
    rook_magics[12] = 2306687503081539584;
    rook_magics[13] = 9288691411454208;
    rook_magics[14] = 181269893860361224;
    rook_magics[15] = 306807729203449922;
    rook_magics[16] = 1459169028051312770;
    rook_magics[17] = 292734250996752384;
    rook_magics[18] = 4614220393136398336;
    rook_magics[19] = 144397762709164032;
    rook_magics[20] = 149747436590989440;
    rook_magics[21] = 1297319267481681928;
    rook_magics[22] = 117097988626727320;
    rook_magics[23] = 597010624714711172;
    rook_magics[24] = 36046665156665346;
    rook_magics[25] = 40532672597991429;
    rook_magics[26] = 180179173763334400;
    rook_magics[27] = 2353167093480304640;
    rook_magics[28] = 589127199270371604;
    rook_magics[29] = 360850945914179656;
    rook_magics[30] = 2305851822487634433;
    rook_magics[31] = 4765130038678932481;
    rook_magics[32] = 144153396448854144;
    rook_magics[33] = 105621839937604;
    rook_magics[34] = 1233987191294664832;
    rook_magics[35] = 734192361113913600;
    rook_magics[36] = 41782012284934;
    rook_magics[37] = 1152956843732045888;
    rook_magics[38] = 3659191960995328;
    rook_magics[39] = 72339361642842241;
    rook_magics[40] = 900722261948907521;
    rook_magics[41] = 2598579321459195904;
    rook_magics[42] = 261218674583601168;
    rook_magics[43] = 4652219514797228048;
    rook_magics[44] = 282059092590672;
    rook_magics[45] = 562967267639440;
    rook_magics[46] = 18665343770427394;
    rook_magics[47] = 1378119920520396801;
    rook_magics[48] = 577094106040238208;
    rook_magics[49] = 36310555924898304;
    rook_magics[50] = 2454534437738645248;
    rook_magics[51] = 81073727931961856;
    rook_magics[52] = 7068400714737780992;
    rook_magics[53] = 2389722591196878336;
    rook_magics[54] = 4683744780738890752;
    rook_magics[55] = 306245341601268224;
    rook_magics[56] = 4629771340269881602;
    rook_magics[57] = 6935543563996430465;
    rook_magics[58] = 1152940196841406473;
    rook_magics[59] = 13229037191425;
    rook_magics[60] = 563018815774722;
    rook_magics[61] = 844596762640403;
    rook_magics[62] = 1152926457510627332;
    rook_magics[63] = 9025117866754658;

    for(Square square = LNX_SQUARE_A1; square <= LNX_SQUARE_H8; ++square)
    {
        if(!init_bishop_attack_board(square))
        {
            LNX_VERIFY(LNX_FALSE);
        }

        if(!init_rook_attack_board(square))
        {
            LNX_VERIFY(LNX_FALSE);
        }
    }
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