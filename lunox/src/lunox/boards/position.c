#include "position.h"

#include <stdio.h>

#include "lunox/boards/board.h"

void side_calculate_occupancy(Side* side)
{
    LNX_ASSERT(side != NULL);
    
    side->occupancy = side->pawns | side->knights | side->bishops | side->rooks | side->queens | side->kings;
}

void position_parse_fen(Position* pos, const char* fen)
{
    LNX_ASSERT(pos != NULL);

    LNX_ASSERT(fen != NULL);
}

void position_startpos(Position* pos)
{
    pos->side_to_move = LNX_SIDE_WHITE;
    
    pos->castling_perms = LNX_CASTLING_PERM_WHITE_KINGSIDE | LNX_CASTLING_PERM_WHITE_QUEENSIDE | LNX_CASTLING_PERM_BLACK_KINGSIDE | LNX_CASTLING_PERM_BLACK_QUEENSIDE;

    Side* white = &pos->sides[LNX_SIDE_WHITE];
    Side* black = &pos->sides[LNX_SIDE_BLACK];

    white->pawns = LNX_BITBOARD_RANK_2;
    white->knights = LNX_BIT(LNX_SQUARE_B1) | LNX_BIT(LNX_SQUARE_G1);
    white->bishops = LNX_BIT(LNX_SQUARE_C1) | LNX_BIT(LNX_SQUARE_F1);
    white->rooks = LNX_BIT(LNX_SQUARE_A1) | LNX_BIT(LNX_SQUARE_H1);
    white->queens = LNX_BIT(LNX_SQUARE_D1);
    white->kings = LNX_BIT(LNX_SQUARE_E1);

    black->pawns = LNX_BITBOARD_RANK_7;
    black->knights = LNX_BIT(LNX_SQUARE_B8) | LNX_BIT(LNX_SQUARE_G8);
    black->bishops = LNX_BIT(LNX_SQUARE_C8) | LNX_BIT(LNX_SQUARE_F8);
    black->rooks = LNX_BIT(LNX_SQUARE_A8) | LNX_BIT(LNX_SQUARE_H8);
    black->queens = LNX_BIT(LNX_SQUARE_D8);
    black->kings = LNX_BIT(LNX_SQUARE_E8);

    side_calculate_occupancy(white);
    side_calculate_occupancy(black);

    position_calculate_occupancy(pos);
}

void position_calculate_occupancy(Position* pos)
{
    LNX_ASSERT(pos != NULL);

    pos->occupancy = pos->sides[LNX_SIDE_WHITE].occupancy | pos->sides[LNX_SIDE_BLACK].occupancy;
}

void position_make_move(Position* pos, Move move)
{
    LNX_ASSERT(pos != NULL);
    
    LNX_VERIFY(position_validate(pos));

    Side* white = &pos->sides[LNX_SIDE_WHITE];
    Side* black = &pos->sides[LNX_SIDE_BLACK];

    const Square from = LNX_MOVE_GET_FROM(move);
    const Square to = LNX_MOVE_GET_TO(move);

    const Bitboard from_bitboard = LNX_BIT(from);
    const Bitboard to_bitboard = LNX_BIT(to);

    switch(LNX_MOVE_GET_TYPE(move))
    {
        case LNX_MOVE_TYPE_NORMAL:
        {
            if(from == LNX_SQUARE_A1)
                LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_WHITE_QUEENSIDE);

            if(from == LNX_SQUARE_H1)
                LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_WHITE_KINGSIDE);

            if(from == LNX_SQUARE_E1)
                LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_WHITE_KINGSIDE | LNX_CASTLING_PERM_WHITE_QUEENSIDE);

            if(from == LNX_SQUARE_A8)
                LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_WHITE_QUEENSIDE);

            if(from == LNX_SQUARE_H8)
                LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_WHITE_KINGSIDE);

            if(from == LNX_SQUARE_E8)
                LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_BLACK_KINGSIDE | LNX_CASTLING_PERM_BLACK_QUEENSIDE);

            white->pawns = white->pawns & from_bitboard ? ((white->pawns | to_bitboard) & ~from_bitboard) : white->pawns;
            white->knights = white->knights & from_bitboard ? ((white->knights | to_bitboard) & ~from_bitboard) : white->knights;
            white->bishops = white->bishops & from_bitboard ? ((white->bishops | to_bitboard) & ~from_bitboard) : white->bishops;
            white->rooks = white->rooks & from_bitboard ? ((white->rooks | to_bitboard) & ~from_bitboard) : white->rooks;
            white->queens = white->queens & from_bitboard ? ((white->queens | to_bitboard) & ~from_bitboard) : white->queens;
            white->kings = white->kings & from_bitboard ? ((white->kings | to_bitboard) & ~from_bitboard) : white->kings;

            black->pawns = black->pawns & from_bitboard ? ((black->pawns | to_bitboard) & ~from_bitboard) : black->pawns;
            black->knights = black->knights & from_bitboard ? ((black->knights | to_bitboard) & ~from_bitboard) : black->knights;
            black->bishops = black->bishops & from_bitboard ? ((black->bishops | to_bitboard) & ~from_bitboard) : black->bishops;
            black->rooks = black->rooks & from_bitboard ? ((black->rooks | to_bitboard) & ~from_bitboard) : black->rooks;
            black->queens = black->queens & from_bitboard ? ((black->queens | to_bitboard) & ~from_bitboard) : black->queens;
            black->kings = black->kings & from_bitboard ? ((black->kings | to_bitboard) & ~from_bitboard) : black->kings;

            break;
        }

        case LNX_MOVE_TYPE_PROMOTION:
        {
            white->pawns &= ~from_bitboard;
            black->pawns &= ~from_bitboard;

            const PromotionPieceType type = LNX_MOVE_GET_PROMOTION_PIECE(move);

            switch(type)
            {
                case LNX_PROMOTION_PIECE_TYPE_KNIGHT:
                {
                    white->knights = pos->side_to_move ? white->knights : (white->knights | to_bitboard);
                    black->knights = pos->side_to_move ? (black->knights | to_bitboard) : black->knights;

                    break;
                }

                case LNX_PROMOTION_PIECE_TYPE_BISHOP:
                {
                    white->bishops = pos->side_to_move ? white->bishops : (white->bishops | to_bitboard);
                    black->bishops = pos->side_to_move ? (black->bishops | to_bitboard) : black->bishops;

                    break;
                }

                case LNX_PROMOTION_PIECE_TYPE_ROOK:
                {
                    white->rooks = pos->side_to_move ? white->rooks : (white->rooks | to_bitboard);
                    black->rooks = pos->side_to_move ? (black->rooks | to_bitboard) : black->rooks;

                    break;
                }

                case LNX_PROMOTION_PIECE_TYPE_QUEEN:
                {
                    white->queens = pos->side_to_move ? white->queens : (white->queens | to_bitboard);
                    black->queens = pos->side_to_move ? (black->queens | to_bitboard) : black->queens;

                    break;
                }

                default: LNX_ASSERT(LNX_FALSE);
            }
            
            break;
        }

        case LNX_MOVE_TYPE_EN_PASSANT:
        {
            white->pawns = pos->side_to_move ? (white->pawns & ~(to_bitboard << LNX_BOARD_WIDTH)) : ((white->pawns & ~from_bitboard) | to_bitboard);
            black->pawns = pos->side_to_move ? ((black->pawns & ~from_bitboard) | to_bitboard) : (black->pawns & ~(to_bitboard >> LNX_BOARD_WIDTH));

            break;
        }

        case LNX_MOVE_TYPE_CASTLING:
        {
            white->kings = pos->side_to_move ? white->kings : ((white->kings & ~LNX_BIT(LNX_SQUARE_E1)) | to_bitboard);
            black->kings = pos->side_to_move ? ((black->kings & ~LNX_BIT(LNX_SQUARE_E8)) | to_bitboard) : black->kings;

            LNX_VERIFY((from == LNX_SQUARE_E1 && (to == LNX_SQUARE_C1 || to == LNX_SQUARE_G1)) || (from == LNX_SQUARE_E8 && (to == LNX_SQUARE_C8 || to == LNX_SQUARE_G8)));

            if(LNX_SQUARE_TO_FILE(to) == LNX_FILE_G)
            {
                LNX_VERIFY(pos->castling_perms & (LNX_CASTLING_PERM_WHITE_KINGSIDE | LNX_CASTLING_PERM_BLACK_KINGSIDE));
                white->rooks = pos->side_to_move ? white->rooks : ((white->rooks & ~LNX_BIT(LNX_SQUARE_H1)) | LNX_BIT(LNX_SQUARE_F1));
                black->rooks = pos->side_to_move ? ((black->rooks & ~LNX_BIT(LNX_SQUARE_H8)) | LNX_BIT(LNX_SQUARE_F8)) : black->rooks;
            }
            else
            {
                LNX_VERIFY(pos->castling_perms & (LNX_CASTLING_PERM_WHITE_QUEENSIDE | LNX_CASTLING_PERM_BLACK_QUEENSIDE));
                white->rooks = pos->side_to_move ? white->rooks : ((white->rooks & ~LNX_BIT(LNX_SQUARE_A1)) | LNX_BIT(LNX_SQUARE_D1));
                black->rooks = pos->side_to_move ? ((black->rooks & ~LNX_BIT(LNX_SQUARE_A8)) | LNX_BIT(LNX_SQUARE_D8)) : black->rooks;
            }

            if(pos->side_to_move == LNX_SIDE_WHITE)
            {
                LNX_VERIFY(pos->castling_perms & (LNX_CASTLING_PERM_WHITE_KINGSIDE | LNX_CASTLING_PERM_WHITE_QUEENSIDE));
                LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_WHITE_KINGSIDE | LNX_CASTLING_PERM_WHITE_QUEENSIDE);
            }
            else
            {
                LNX_VERIFY(pos->castling_perms & (LNX_CASTLING_PERM_BLACK_KINGSIDE | LNX_CASTLING_PERM_BLACK_QUEENSIDE));
                LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_BLACK_KINGSIDE | LNX_CASTLING_PERM_BLACK_QUEENSIDE);
            }

            break;
        }

        default: LNX_ASSERT(LNX_FALSE);
    }

    side_calculate_occupancy(white);
    side_calculate_occupancy(black);

    position_calculate_occupancy(pos);

    pos->side_to_move = !pos->side_to_move;

    LNX_VERIFY(position_validate(pos));
}

LunoxBool position_validate(const Position* pos)
{
    LNX_ASSERT(pos != NULL);

    if(pos->side_to_move != LNX_SIDE_WHITE && pos->side_to_move != LNX_SIDE_BLACK)
        return LNX_FALSE;

    const Side* white = &pos->sides[LNX_SIDE_WHITE];
    const Side* black = &pos->sides[LNX_SIDE_BLACK];

    if(pos->occupancy != (white->occupancy | black->occupancy))
        return LNX_FALSE;

    if(white->occupancy & black->occupancy)
        return LNX_FALSE;

    if(white->occupancy != (white->pawns | white->knights | white->bishops | white->rooks | white->queens | white->kings))
        return LNX_FALSE;

    if(black->occupancy != (black->pawns | black->knights | black->bishops | black->rooks | black->queens | black->kings))
        return LNX_FALSE;

    if(white->pawns & (white->knights | white->bishops | white->rooks | white->queens | white->kings))
        return LNX_FALSE;

    if(white->knights & (white->bishops | white->rooks | white->queens | white->kings))
        return LNX_FALSE;

    if(white->bishops & (white->rooks | white->queens | white->kings))
        return LNX_FALSE;

    if(white->rooks & (white->queens | white->kings))
        return LNX_FALSE;

    if(white->queens & white->kings)
        return LNX_FALSE;

    if(black->pawns & (black->knights | black->bishops | black->rooks | black->queens | black->kings))
        return LNX_FALSE;

    if(black->knights & (black->bishops | black->rooks | black->queens | black->kings))
        return LNX_FALSE;

    if(black->bishops & (black->rooks | black->queens | black->kings))
        return LNX_FALSE;

    if(black->rooks & (black->queens | black->kings))
        return LNX_FALSE;

    if(black->queens & black->kings)
        return LNX_FALSE;

    return LNX_TRUE;
}

static char square_to_char(const Position* pos, Square square)
{
    LNX_ASSERT(pos != NULL);
    
    LNX_VERIFY(square < LNX_SQUARE_OFFBOARD);

    const Bitboard square_bitboard = LNX_BIT(square);

    const Side* white = &pos->sides[LNX_SIDE_WHITE];
    const Side* black = &pos->sides[LNX_SIDE_BLACK];

    if(white->pawns & square_bitboard)
        return 'P';

    if(black->pawns & square_bitboard)
        return 'p';
        
    if(white->knights & square_bitboard)
        return 'N';

    if(black->knights & square_bitboard)
        return 'n';

    if(white->bishops & square_bitboard)
        return 'B';

    if(black->bishops & square_bitboard)
        return 'b';

    if(white->rooks & square_bitboard)
        return 'R';

    if(black->rooks & square_bitboard)
        return 'r';

    if(white->queens & square_bitboard)
        return 'Q';

    if(black->queens & square_bitboard)
        return 'q';

    if(white->kings & square_bitboard)
        return 'K';

    if(black->kings & square_bitboard)
        return 'k';

    return ' ';
}

static void position_print_white(const Position* pos)
{
    LNX_ASSERT(pos != NULL);

    for(uint8_t y = 0; y < LNX_BOARD_HEIGHT; ++y)
    {
        printf("  +---+---+---+---+---+---+---+---+\n");

        printf("%c ", '8' - y);

        for(uint8_t x = 0; x < LNX_BOARD_WIDTH; ++x)
            printf("| %c ", square_to_char(pos, (LNX_BOARD_HEIGHT - 1 - y) * LNX_BOARD_WIDTH + x));

        printf("|\n");
    }

    printf("  +---+---+---+---+---+---+---+---+\n");
    printf("    a   b   c   d   e   f   g   h\n");
}

static void position_print_black(const Position* pos)
{
    LNX_ASSERT(pos != NULL);

    for(uint8_t y = 0; y < LNX_BOARD_HEIGHT; ++y)
    {
        printf("  +---+---+---+---+---+---+---+---+\n");

        printf("%c ", '1' + y);

        for(uint8_t x = 0; x < LNX_BOARD_WIDTH; ++x)
        {
            printf("| %c ", square_to_char(pos, y * LNX_BOARD_WIDTH + (LNX_BOARD_WIDTH - 1 - x)));
        }

        printf("|\n");
    }

    printf("  +---+---+---+---+---+---+---+---+\n");
    printf("    h   g   f   e   d   c   b   a\n");
}

void position_print(const Position* pos, uint8_t side)
{
    LNX_ASSERT(pos != NULL);

    switch(side)
    {
        case LNX_SIDE_WHITE: position_print_white(pos); break;
        case LNX_SIDE_BLACK: position_print_black(pos); break;

        default: LNX_ASSERT(LNX_FALSE);
    }
}