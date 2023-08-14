#include "position.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lunox/boards/board.h"

void side_calculate_occupancy(Side* side)
{
    LNX_ASSERT(side != NULL);
    
    side->occupancy = side->pawns | side->knights | side->bishops | side->rooks | side->queens | side->kings;
}

LunoxBool position_parse_fen(Position* pos, const char* fen)
{
    LNX_ASSERT(pos != NULL);

    LNX_ASSERT(fen != NULL);

    Side* white = &pos->sides[LNX_SIDE_WHITE];
    Side* black = &pos->sides[LNX_SIDE_BLACK];

    white->pawns = LNX_BITBOARD_EMPTY;
    white->knights = LNX_BITBOARD_EMPTY;
    white->bishops = LNX_BITBOARD_EMPTY;
    white->rooks = LNX_BITBOARD_EMPTY;
    white->queens = LNX_BITBOARD_EMPTY;
    white->kings = LNX_BITBOARD_EMPTY;

    black->pawns = LNX_BITBOARD_EMPTY;
    black->knights = LNX_BITBOARD_EMPTY;
    black->bishops = LNX_BITBOARD_EMPTY;
    black->rooks = LNX_BITBOARD_EMPTY;
    black->queens = LNX_BITBOARD_EMPTY;
    black->kings = LNX_BITBOARD_EMPTY;

    move_history_reset(&pos->history);

#define LNX_FEN_CAPACITY 128
    char fen_buffer[LNX_FEN_CAPACITY];
    strncpy(fen_buffer, fen, sizeof(fen_buffer) - 1);
    fen_buffer[LNX_FEN_CAPACITY - 1] = '\0';

    int8_t file = LNX_FILE_A;
    int8_t rank = LNX_RANK_8;

    char* token = strtok(fen_buffer, " ");
    while(*token)
    {
        Bitboard square_bitboard = LNX_BIT(LNX_FILE_RANK_TO_SQUARE(file, rank));

        switch(*token)
        {
            case 'P': ++file; white->pawns |= square_bitboard; break;
            case 'N': ++file; white->knights |= square_bitboard; break;
            case 'B': ++file; white->bishops |= square_bitboard; break;
            case 'R': ++file; white->rooks |= square_bitboard; break;
            case 'Q': ++file; white->queens |= square_bitboard; break;
            case 'K': ++file; white->kings |= square_bitboard; break;

            case 'p': ++file; black->pawns |= square_bitboard; break;
            case 'n': ++file; black->knights |= square_bitboard; break;
            case 'b': ++file; black->bishops |= square_bitboard; break;
            case 'r': ++file; black->rooks |= square_bitboard; break;
            case 'q': ++file; black->queens |= square_bitboard; break;
            case 'k': ++file; black->kings |= square_bitboard; break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            {
                uint8_t count = *token - '0';
                file += count;
                break;
            }

            case '/':
            {
                --rank;
                file = LNX_FILE_A;
                ++token;
                continue;
            }

            default:  printf("Invalid FEN: Could not parse piece position character '%c'!\n", *token); return LNX_FALSE;
        }

        ++token;
    }

    token = strtok(NULL, " ");

    switch(*token)
    {
        case 'w': pos->side_to_move = LNX_SIDE_WHITE; break;
        case 'b': pos->side_to_move = LNX_SIDE_BLACK; break;

        default:  printf("Invalid FEN: Could not parse side to move character '%c'!\n", *token); return LNX_FALSE;
    }

    pos->castling_perms = LNX_CASTLING_PERM_NONE;
    token = strtok(NULL, " ");
    while(*token && *token != '-')
    {
        switch(*token)
        {
            case 'K': pos->castling_perms |= LNX_CASTLING_PERM_WHITE_KINGSIDE; break;
            case 'Q': pos->castling_perms |= LNX_CASTLING_PERM_WHITE_QUEENSIDE; break;
            case 'k': pos->castling_perms |= LNX_CASTLING_PERM_BLACK_KINGSIDE; break;
            case 'q': pos->castling_perms |= LNX_CASTLING_PERM_BLACK_QUEENSIDE; break;

            default:  printf("Invalid FEN: Could not parse castling permission character '%c'!\n", *token); return LNX_FALSE;
        }

        ++token;
    }

    side_calculate_occupancy(white);
    side_calculate_occupancy(black);

    position_calculate_occupancy(pos);

    token = strtok(NULL, " ");
    if(*token == '-' && token[1] == '\0')
        pos->ep_square = LNX_SQUARE_OFFBOARD;
    else if(token[0] >= 'a' && token[0] <= 'h' && token[1] >= '1' && token[1] <= '8' && token[2] == '\0')
        pos->ep_square = LNX_FILE_RANK_TO_SQUARE(token[0] - 'a', token[1] - '1');
    else
    {
        printf("Invalid FEN: Could not parse en passant square \"%s\"!\n", token);
        return LNX_FALSE;
    }

    token = strtok(NULL, " ");
    int fifty_move_rule = atoi(token);
    if(fifty_move_rule < 0)
    {
        printf("Invalid FEN: Half move clock cannot be negative!\n");
        return LNX_FALSE;
    }

    pos->fifty_move_rule = fifty_move_rule;

    token = strtok(NULL, " ");
    int full_move = atoi(token);
    if(full_move < 1)
    {
        printf("Invalid FEN: Full move clock needs to be at least 1!\n");
        return LNX_FALSE;
    }

    pos->plys = 2 * (full_move - 1) + pos->side_to_move == LNX_SIDE_WHITE ? 0 : 1;

    LNX_VERIFY(position_validate(pos));

    return LNX_TRUE;
}

void position_startpos(Position* pos)
{
    LNX_ASSERT(pos != NULL);
    
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
    
    pos->castling_perms = LNX_CASTLING_PERM_WHITE_KINGSIDE | LNX_CASTLING_PERM_WHITE_QUEENSIDE | LNX_CASTLING_PERM_BLACK_KINGSIDE | LNX_CASTLING_PERM_BLACK_QUEENSIDE;

    pos->ep_square = LNX_SQUARE_OFFBOARD;

    pos->fifty_move_rule = 0;

    pos->plys = 0;

    pos->side_to_move = LNX_SIDE_WHITE;

    move_history_reset(&pos->history);
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

    pos->ep_square = LNX_SQUARE_OFFBOARD;

    Side* white = &pos->sides[LNX_SIDE_WHITE];
    Side* black = &pos->sides[LNX_SIDE_BLACK];

    Square from = LNX_MOVE_GET_FROM(move);
    Square to = LNX_MOVE_GET_TO(move);

    Bitboard from_bitboard = LNX_BIT(from);
    Bitboard to_bitboard = LNX_BIT(to);

    MoveHistoryEntry entry;
    entry.move = move;
    entry.castling_perms = pos->castling_perms;
    entry.fifty_move_rule = pos->fifty_move_rule;
    entry.captured_piece_type = LNX_PIECE_TYPE_NONE;

    ++pos->fifty_move_rule;

    const Side* enemy = &pos->sides[!pos->side_to_move];
    if(enemy->occupancy & to_bitboard)
    {
        if(enemy->pawns & to_bitboard)
            entry.captured_piece_type = LNX_PIECE_TYPE_PAWN;

        if(enemy->knights & to_bitboard)
            entry.captured_piece_type = LNX_PIECE_TYPE_KNIGHT;

        if(enemy->bishops & to_bitboard)
            entry.captured_piece_type = LNX_PIECE_TYPE_BISHOP;

        if(enemy->rooks & to_bitboard)
            entry.captured_piece_type = LNX_PIECE_TYPE_ROOK;

        if(enemy->queens & to_bitboard)
            entry.captured_piece_type = LNX_PIECE_TYPE_QUEEN;

        LNX_VERIFY(!(enemy->kings & to_bitboard));
    }

    move_history_push(&pos->history, entry);

    if(to_bitboard & pos->occupancy || from_bitboard & (white->pawns | black->pawns))
        pos->fifty_move_rule = 0;

    switch(to)
    {
        case LNX_SQUARE_A1: LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_WHITE_QUEENSIDE); break;
        case LNX_SQUARE_H1: LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_WHITE_KINGSIDE); break;
        case LNX_SQUARE_A8: LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_BLACK_QUEENSIDE); break;
        case LNX_SQUARE_H8: LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_BLACK_KINGSIDE); break;

        default: break;
    }

    switch(LNX_MOVE_GET_TYPE(move))
    {
        case LNX_MOVE_TYPE_NORMAL:
        {
            switch(from)
            {
                case LNX_SQUARE_A1: LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_WHITE_QUEENSIDE); break;
                case LNX_SQUARE_E1: LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_WHITE_KINGSIDE | LNX_CASTLING_PERM_WHITE_QUEENSIDE); break;
                case LNX_SQUARE_H1: LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_WHITE_KINGSIDE); break;
                case LNX_SQUARE_A8: LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_BLACK_QUEENSIDE); break;
                case LNX_SQUARE_E8: LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_BLACK_KINGSIDE | LNX_CASTLING_PERM_BLACK_QUEENSIDE); break;
                case LNX_SQUARE_H8: LNX_BIT_CLEAR(pos->castling_perms, LNX_CASTLING_PERM_BLACK_KINGSIDE); break;

                default: break;
            }

            if(white->pawns & from_bitboard && to_bitboard == from_bitboard << 2 * LNX_BOARD_WIDTH)
                pos->ep_square = from + LNX_BOARD_WIDTH;
                
            if(black->pawns & from_bitboard && to_bitboard == from_bitboard >> 2 * LNX_BOARD_WIDTH)
                pos->ep_square = from - LNX_BOARD_WIDTH;

            white->pawns = white->pawns & from_bitboard ? ((white->pawns | to_bitboard) & ~from_bitboard) : white->pawns & ~to_bitboard;
            white->knights = white->knights & from_bitboard ? ((white->knights | to_bitboard) & ~from_bitboard) : white->knights & ~to_bitboard;
            white->bishops = white->bishops & from_bitboard ? ((white->bishops | to_bitboard) & ~from_bitboard) : white->bishops & ~to_bitboard;
            white->rooks = white->rooks & from_bitboard ? ((white->rooks | to_bitboard) & ~from_bitboard) : white->rooks & ~to_bitboard;
            white->queens = white->queens & from_bitboard ? ((white->queens | to_bitboard) & ~from_bitboard) : white->queens & ~to_bitboard;
            white->kings = white->kings & from_bitboard ? ((white->kings | to_bitboard) & ~from_bitboard) : white->kings & ~to_bitboard;

            black->pawns = black->pawns & from_bitboard ? ((black->pawns | to_bitboard) & ~from_bitboard) : black->pawns & ~to_bitboard;
            black->knights = black->knights & from_bitboard ? ((black->knights | to_bitboard) & ~from_bitboard) : black->knights & ~to_bitboard;
            black->bishops = black->bishops & from_bitboard ? ((black->bishops | to_bitboard) & ~from_bitboard) : black->bishops & ~to_bitboard;
            black->rooks = black->rooks & from_bitboard ? ((black->rooks | to_bitboard) & ~from_bitboard) : black->rooks & ~to_bitboard;
            black->queens = black->queens & from_bitboard ? ((black->queens | to_bitboard) & ~from_bitboard) : black->queens & ~to_bitboard;
            black->kings = black->kings & from_bitboard ? ((black->kings | to_bitboard) & ~from_bitboard) : black->kings & ~to_bitboard;

            break;
        }

        case LNX_MOVE_TYPE_PROMOTION:
        {
            white->pawns &= ~from_bitboard;
            black->pawns &= ~from_bitboard;

            white->knights &= ~to_bitboard;
            white->bishops &= ~to_bitboard;
            white->rooks &= ~to_bitboard;
            white->queens &= ~to_bitboard;

            black->knights &= ~to_bitboard;
            black->bishops &= ~to_bitboard;
            black->rooks &= ~to_bitboard;
            black->queens &= ~to_bitboard;

            PromotionPieceType type = LNX_MOVE_GET_PROMOTION_PIECE(move);

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

    ++pos->plys;

    pos->side_to_move = !pos->side_to_move;

    LNX_VERIFY(position_validate(pos));
}

void position_undo_move(Position* pos)
{
    LNX_ASSERT(pos != NULL);

    LNX_VERIFY(position_validate(pos));

    pos->ep_square = LNX_SQUARE_OFFBOARD;

    --pos->plys;

    pos->side_to_move = !pos->side_to_move;

    Side* white = &pos->sides[LNX_SIDE_WHITE];
    Side* black = &pos->sides[LNX_SIDE_BLACK];

    Side* side = &pos->sides[pos->side_to_move];
    Side* enemy = &pos->sides[!pos->side_to_move];

    MoveHistoryEntry entry = move_history_pop(&pos->history);
    Move last_move = entry.move;

    pos->castling_perms = entry.castling_perms;

    pos->fifty_move_rule = entry.fifty_move_rule;

    Square from = LNX_MOVE_GET_FROM(last_move);
    Square to = LNX_MOVE_GET_TO(last_move);
    MoveType type = LNX_MOVE_GET_TYPE(last_move);

    Bitboard from_bitboard = LNX_BIT(from);
    Bitboard to_bitboard = LNX_BIT(to);

    switch(type)
    {
        case LNX_MOVE_TYPE_NORMAL:
        {
            side->pawns = side->pawns & to_bitboard ? side->pawns | from_bitboard : side->pawns;
            side->knights = side->knights & to_bitboard ? side->knights | from_bitboard : side->knights;
            side->bishops = side->bishops & to_bitboard ? side->bishops | from_bitboard : side->bishops;
            side->rooks = side->rooks & to_bitboard ? side->rooks | from_bitboard : side->rooks;
            side->queens = side->queens & to_bitboard ? side->queens | from_bitboard : side->queens;
            side->kings = side->kings & to_bitboard ? side->kings | from_bitboard : side->kings;
            
            break;
        }

        case LNX_MOVE_TYPE_CASTLING:
        {
            switch(to)
            {
                case LNX_SQUARE_C1:
                {
                    LNX_BIT_SET(side->kings, LNX_BIT(LNX_SQUARE_E1));
                    LNX_BIT_SET(side->rooks, LNX_BIT(LNX_SQUARE_A1));
                    LNX_BIT_CLEAR(side->rooks, LNX_BIT(LNX_SQUARE_D1));

                    break;
                }

                case LNX_SQUARE_G1:
                {
                    LNX_BIT_SET(side->kings, LNX_BIT(LNX_SQUARE_E1));
                    LNX_BIT_SET(side->rooks, LNX_BIT(LNX_SQUARE_H1));
                    LNX_BIT_CLEAR(side->rooks, LNX_BIT(LNX_SQUARE_F1));
                    
                    break;
                }

                case LNX_SQUARE_C8:
                {
                    LNX_BIT_SET(side->kings, LNX_BIT(LNX_SQUARE_E8));
                    LNX_BIT_SET(side->rooks, LNX_BIT(LNX_SQUARE_A8));
                    LNX_BIT_CLEAR(side->rooks, LNX_BIT(LNX_SQUARE_D8));
                    
                    break;
                }

                case LNX_SQUARE_G8:
                {
                    LNX_BIT_SET(side->kings, LNX_BIT(LNX_SQUARE_E8));
                    LNX_BIT_SET(side->rooks, LNX_BIT(LNX_SQUARE_H8));
                    LNX_BIT_CLEAR(side->rooks, LNX_BIT(LNX_SQUARE_F8));
                    
                    break;
                }

                default: LNX_ASSERT(LNX_FALSE); break;
            }
            
            break;
        }

        case LNX_MOVE_TYPE_PROMOTION:
        {
            LNX_BIT_SET(side->pawns, from_bitboard);

            break;
        }

        case LNX_MOVE_TYPE_EN_PASSANT:
        {
            pos->ep_square = to;

            LNX_BIT_SET(side->pawns, from_bitboard);

            if(pos->side_to_move == LNX_SIDE_WHITE)
                LNX_BIT_SET(enemy->pawns, LNX_BIT(pos->ep_square - LNX_BOARD_WIDTH));
            else
                LNX_BIT_SET(enemy->pawns, LNX_BIT(pos->ep_square + LNX_BOARD_WIDTH));

            break;
        }

        default: LNX_ASSERT(LNX_FALSE); break;
    }

    switch(entry.captured_piece_type)
    {
        case LNX_PIECE_TYPE_NONE: break;

        case LNX_PIECE_TYPE_PAWN: LNX_BIT_SET(enemy->pawns, to_bitboard); break;
        case LNX_PIECE_TYPE_KNIGHT: LNX_BIT_SET(enemy->knights, to_bitboard); break;
        case LNX_PIECE_TYPE_BISHOP: LNX_BIT_SET(enemy->bishops, to_bitboard); break;
        case LNX_PIECE_TYPE_ROOK: LNX_BIT_SET(enemy->rooks, to_bitboard); break;
        case LNX_PIECE_TYPE_QUEEN: LNX_BIT_SET(enemy->queens, to_bitboard); break;

        default: LNX_ASSERT(LNX_FALSE); break;
    }

    LNX_BIT_CLEAR(side->pawns, to_bitboard);
    LNX_BIT_CLEAR(side->knights, to_bitboard);
    LNX_BIT_CLEAR(side->bishops, to_bitboard);
    LNX_BIT_CLEAR(side->rooks, to_bitboard);
    LNX_BIT_CLEAR(side->queens, to_bitboard);
    LNX_BIT_CLEAR(side->kings, to_bitboard);

    side_calculate_occupancy(white);
    side_calculate_occupancy(black);

    position_calculate_occupancy(pos);

    LNX_VERIFY(position_validate(pos));
}

uint8_t position_attack_count_on_square(const Position* pos, uint8_t side, Square square)
{
    LNX_ASSERT(pos != NULL);

    LNX_VERIFY(square < LNX_SQUARE_OFFBOARD);

    Bitboard attackers = LNX_BITBOARD_EMPTY;

    const Side* attacker_side = &pos->sides[side];

    attackers |= (side == LNX_SIDE_WHITE ? black_pawn_attacks[square] : white_pawn_attacks[square]) & attacker_side->pawns;
    attackers |= knight_attacks[square] & attacker_side->knights;
    attackers |= king_attacks[square] & attacker_side->kings;
    attackers |= bitboard_get_bishop_attacks(square, pos->occupancy) & attacker_side->bishops;
    attackers |= bitboard_get_rook_attacks(square, pos->occupancy) & attacker_side->rooks;
    attackers |= bitboard_get_queen_attacks(square, pos->occupancy) & attacker_side->queens;

    return LNX_BIT_COUNT(attackers);
}

LunoxBool position_validate(const Position* pos)
{
    LNX_ASSERT(pos != NULL);

    if(pos->ep_square > LNX_SQUARE_OFFBOARD)
    {
        printf("[ERROR]: Invalid en passant square!\n");
        return LNX_FALSE;
    }

    if(pos->side_to_move != LNX_SIDE_WHITE && pos->side_to_move != LNX_SIDE_BLACK)
    {
        printf("[ERROR]: Invalid side to move!\n");
        return LNX_FALSE;
    }

    const Side* white = &pos->sides[LNX_SIDE_WHITE];
    const Side* black = &pos->sides[LNX_SIDE_BLACK];

    if(pos->castling_perms & (LNX_CASTLING_PERM_WHITE_KINGSIDE | LNX_CASTLING_PERM_WHITE_QUEENSIDE) && ~white->kings & LNX_BIT(LNX_SQUARE_E1))
    {
        printf("[ERROR]: White has still castling permissions, however there is no king on e1!\n");
        return LNX_FALSE;
    }

    if(pos->castling_perms & LNX_CASTLING_PERM_WHITE_KINGSIDE && ~white->rooks & LNX_BIT(LNX_SQUARE_H1))
    {
        printf("[ERROR]: White has still castling permission to castle kingside, however there is no rook on h1!\n");
        return LNX_FALSE;
    }

    if(pos->castling_perms & LNX_CASTLING_PERM_WHITE_QUEENSIDE && ~white->rooks & LNX_BIT(LNX_SQUARE_A1))
    {
        printf("[ERROR]: White has still castling permission to castle queenside, however there is no rook on a1!\n");
        return LNX_FALSE;
    }

    if(pos->castling_perms & (LNX_CASTLING_PERM_BLACK_KINGSIDE | LNX_CASTLING_PERM_BLACK_QUEENSIDE) && ~black->kings & LNX_BIT(LNX_SQUARE_E8))
    {
        printf("[ERROR]: Black has still castling permissions, however there is no king on e8!\n");
        return LNX_FALSE;
    }

    if(pos->castling_perms & LNX_CASTLING_PERM_BLACK_KINGSIDE && ~black->rooks & LNX_BIT(LNX_SQUARE_H8))
    {
        printf("[ERROR]: Black has still castling permission to castle kingside, however there is no rook on h8!\n");
        return LNX_FALSE;
    }

    if(pos->castling_perms & LNX_CASTLING_PERM_BLACK_QUEENSIDE && ~black->rooks & LNX_BIT(LNX_SQUARE_A8))
    {
        printf("[ERROR]: Black has still castling permission to castle queenside, however there is no rook on a8!\n");
        return LNX_FALSE;
    }

    if(pos->occupancy != (white->occupancy | black->occupancy))
    {
        printf("[ERROR]: Position's occupancy does not match with white's and black's occupancy!\n");
        return LNX_FALSE;
    }

    if(white->occupancy & black->occupancy)
    {
        printf("[ERROR]: Occupancies of white and black are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->occupancy != (white->pawns | white->knights | white->bishops | white->rooks | white->queens | white->kings))
    {
        printf("[ERROR]: White's occupancy does not match with the occupancies of the white pieces!\n");
        return LNX_FALSE;
    }

    if(black->occupancy != (black->pawns | black->knights | black->bishops | black->rooks | black->queens | black->kings))
    {
        printf("[ERROR]: Black's occupancy does not match with the occupancies of the black pieces!\n");
        return LNX_FALSE;
    }

    if(white->pawns & white->knights)
    {
        printf("[ERROR]: White's pawn and knight occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->pawns & white->bishops)
    {
        printf("[ERROR]: White's pawn and bishop occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->pawns & white->rooks)
    {
        printf("[ERROR]: White's pawn and rook occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->pawns & white->queens)
    {
        printf("[ERROR]: White's pawn and queen occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->pawns & white->kings)
    {
        printf("[ERROR]: White's pawn and king occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->knights & white->bishops)
    {
        printf("[ERROR]: White's knight and bishop occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->knights & white->rooks)
    {
        printf("[ERROR]: White's knight and rook occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->knights & white->queens)
    {
        printf("[ERROR]: White's knight and queen occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->knights & white->kings)
    {
        printf("[ERROR]: White's knight and king occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->bishops & white->rooks)
    {
        printf("[ERROR]: White's bishop and rook occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->bishops & white->queens)
    {
        printf("[ERROR]: White's bishop and queen occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->bishops & white->kings)
    {
        printf("[ERROR]: White's bishop and king occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->rooks & white->queens)
    {
        printf("[ERROR]: White's rook and queen occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->rooks & white->kings)
    {
        printf("[ERROR]: White's rook and king occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(white->queens & white->kings)
    {
        printf("[ERROR]: White's queen and king occupancies are not distinct!\n");
        return LNX_FALSE;
    }
    
    if(black->pawns & black->knights)
    {
        printf("[ERROR]: Black's pawn and knight occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(black->pawns & black->bishops)
    {
        printf("[ERROR]: Black's pawn and bishop occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(black->pawns & black->rooks)
    {
        printf("[ERROR]: Black's pawn and rook occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(black->pawns & black->queens)
    {
        printf("[ERROR]: Black's pawn and queen occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(black->pawns & black->kings)
    {
        printf("[ERROR]: Black's pawn and king occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(black->knights & black->bishops)
    {
        printf("[ERROR]: Black's knight and bishop occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(black->knights & black->rooks)
    {
        printf("[ERROR]: Black's knight and rook occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(black->knights & black->queens)
    {
        printf("[ERROR]: Black's knight and queen occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(black->knights & black->kings)
    {
        printf("[ERROR]: Black's knight and king occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(black->bishops & black->rooks)
    {
        printf("[ERROR]: Black's bishop and rook occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(black->bishops & black->queens)
    {
        printf("[ERROR]: Black's bishop and queen occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(black->bishops & black->kings)
    {
        printf("[ERROR]: Black's bishop and king occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(black->rooks & black->queens)
    {
        printf("[ERROR]: Black's rook and queen occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(black->rooks & black->kings)
    {
        printf("[ERROR]: Black's rook and king occupancies are not distinct!\n");
        return LNX_FALSE;
    }

    if(black->queens & black->kings)
    {
        printf("[ERROR]: Black's queen and king occupancies are not distinct!\n");
        return LNX_FALSE;
    }
    
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

    const Side* enemy = &pos->sides[!pos->side_to_move];

    LNX_VERIFY(enemy->kings != LNX_BITBOARD_EMPTY);
    if(position_attack_count_on_square(pos, pos->side_to_move, LNX_BIT_LSB_INDEX(enemy->kings)) != 0)
    {
        printf("[ERROR]: It's %s to move, however the enemy king is in check!\n", pos->side_to_move == LNX_SIDE_WHITE ? "white" : "black");
        return LNX_FALSE;
    }

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