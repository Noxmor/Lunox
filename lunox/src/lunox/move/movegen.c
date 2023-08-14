#include "movegen.h"

#include "lunox/boards/board.h"

static Bitboard white_king_kingside_castling_bitboard = LNX_BIT(LNX_SQUARE_F1) | LNX_BIT(LNX_SQUARE_G1);
static Bitboard white_king_queenside_castling_bitboard = LNX_BIT(LNX_SQUARE_D1) | LNX_BIT(LNX_SQUARE_C1) | LNX_BIT(LNX_SQUARE_B1);

static Bitboard black_king_kingside_castling_bitboard = LNX_BIT(LNX_SQUARE_F8) | LNX_BIT(LNX_SQUARE_G8);
static Bitboard black_king_queenside_castling_bitboard = LNX_BIT(LNX_SQUARE_D8) | LNX_BIT(LNX_SQUARE_C8) | LNX_BIT(LNX_SQUARE_B8);

static Bitboard calculate_check_mask(const Position* pos)
{
    LNX_ASSERT(pos != NULL);

    Bitboard kings = pos->sides[pos->side_to_move].kings;

    LNX_VERIFY(kings != LNX_BITBOARD_EMPTY);
    Square king_square = LNX_BIT_LSB_INDEX(kings);

    if(position_attack_count_on_square(pos, !pos->side_to_move, king_square) == 0)
        return LNX_BITBOARD_FULL;

    Bitboard check_mask = LNX_BITBOARD_EMPTY;

    const Side* enemy = &pos->sides[!pos->side_to_move];
    Bitboard enemy_pawns = enemy->pawns;
    Bitboard enemy_knights = enemy->knights;
    Bitboard enemy_bishops = enemy->bishops | enemy->queens;
    Bitboard enemy_rooks = enemy->rooks | enemy->queens;

    for(Square from = LNX_SQUARE_A1; from <= LNX_SQUARE_H8; ++from)
    {
        Bitboard from_bitboard = LNX_BIT(from);

        if(enemy_pawns & from_bitboard)
        {
            if((!pos->side_to_move) == LNX_SIDE_WHITE && white_pawn_attacks[from] & kings)
                check_mask |= from_bitboard;
            else if((!pos->side_to_move) == LNX_SIDE_BLACK && black_pawn_attacks[from] & kings)
                check_mask |= from_bitboard;
        }
        
        if(enemy_knights & from_bitboard)
        {
            if(knight_attacks[from] & kings)
                check_mask |= from_bitboard;
        }

        if(enemy_bishops & from_bitboard)
        {
            Bitboard bishop_attacks = bitboard_get_bishop_attacks(from, pos->occupancy);

            if(bishop_attacks & kings)
                check_mask |= (bishop_attacks & bitboard_get_bishop_attacks(king_square, pos->occupancy)) | from_bitboard;
        }

        if(enemy_rooks & from_bitboard)
        {
            Bitboard rook_attacks = bitboard_get_rook_attacks(from, pos->occupancy);

            if(rook_attacks & kings)
                check_mask |= (rook_attacks & bitboard_get_rook_attacks(king_square, pos->occupancy)) | from_bitboard;
        }
    }

    return check_mask;
}

static Bitboard calculate_pin_mask_horizontal_vertical(const Position* pos)
{
    LNX_ASSERT(pos != NULL);

    Bitboard kings = pos->sides[pos->side_to_move].kings;

    LNX_VERIFY(kings != LNX_BITBOARD_EMPTY);
    Square king_square = LNX_BIT_LSB_INDEX(kings);

    Bitboard pin_mask_horizontal_vertical = LNX_BITBOARD_EMPTY;

    const Side* side = &pos->sides[pos->side_to_move];
    const Side* enemy = &pos->sides[!pos->side_to_move];

    Bitboard slider_occupancy = enemy->rooks | enemy->queens;

    while(slider_occupancy)
    {
        Square from = LNX_BIT_LSB_INDEX(slider_occupancy);
        Bitboard from_bitboard = LNX_BIT(from);

        if(bitboard_get_rook_attacks(from, enemy->occupancy) & kings)
        {
            Bitboard ray = bitboard_get_rook_attacks(from, kings) & bitboard_get_rook_attacks(king_square, from_bitboard);

            if(LNX_BIT_COUNT(ray & side->occupancy) < 2)
                pin_mask_horizontal_vertical |= from_bitboard | ray;
        }

        LNX_BIT_LSB_RESET(slider_occupancy);
    }

    return pin_mask_horizontal_vertical;
}

static Bitboard calculate_pin_mask_diagonal(const Position* pos)
{
    LNX_ASSERT(pos != NULL);

    Bitboard kings = pos->sides[pos->side_to_move].kings;

    LNX_VERIFY(kings != LNX_BITBOARD_EMPTY);
    Square king_square = LNX_BIT_LSB_INDEX(kings);

    Bitboard pin_mask_diagonal = LNX_BITBOARD_EMPTY;

    const Side* side = &pos->sides[pos->side_to_move];
    const Side* enemy = &pos->sides[!pos->side_to_move];

    Bitboard slider_occupancy = enemy->bishops | enemy->queens;

    while(slider_occupancy)
    {
        Square from = LNX_BIT_LSB_INDEX(slider_occupancy);
        Bitboard from_bitboard = LNX_BIT(from);

        if(bitboard_get_bishop_attacks(from, enemy->occupancy) & kings)
        {
            Bitboard ray = bitboard_get_bishop_attacks(from, kings) & bitboard_get_bishop_attacks(king_square, from_bitboard);

            if(LNX_BIT_COUNT(ray & side->occupancy) < 2)
                pin_mask_diagonal |= from_bitboard | ray;
        }

        LNX_BIT_LSB_RESET(slider_occupancy);
    }

    return pin_mask_diagonal;
}

static uint8_t position_attack_count_on_square_without_enemy_king(const Position* pos, uint8_t side, Square square)
{
    LNX_ASSERT(pos != NULL);

    LNX_VERIFY(square < LNX_SQUARE_OFFBOARD);

    Bitboard attackers = LNX_BITBOARD_EMPTY;

    const Side* attacker_side = &pos->sides[side];

    Bitboard occupancy = pos->occupancy & ~pos->sides[!side].kings;

    attackers |= (side == LNX_SIDE_WHITE ? black_pawn_attacks[square] : white_pawn_attacks[square]) & attacker_side->pawns;
    attackers |= knight_attacks[square] & attacker_side->knights;
    attackers |= king_attacks[square] & attacker_side->kings;
    attackers |= bitboard_get_bishop_attacks(square, occupancy) & attacker_side->bishops;
    attackers |= bitboard_get_rook_attacks(square, occupancy) & attacker_side->rooks;
    attackers |= bitboard_get_queen_attacks(square, occupancy) & attacker_side->queens;

    return LNX_BIT_COUNT(attackers);
}

void movegen_generate_moves(const Position* pos, MoveList* move_list)
{
    LNX_ASSERT(pos != NULL);

    LNX_ASSERT(move_list != NULL);

    move_list_reset(move_list);

    Bitboard check_mask = calculate_check_mask(pos);

    Bitboard pin_mask_horizontal_vertical = calculate_pin_mask_horizontal_vertical(pos);
    Bitboard pin_mask_diagonal = calculate_pin_mask_diagonal(pos);
    Bitboard pin_mask = pin_mask_horizontal_vertical | pin_mask_diagonal;

    const Side* side = &pos->sides[pos->side_to_move];

    Bitboard kings = side->kings;
    Bitboard pawns = side->pawns;
    Bitboard knights = side->knights;
    Bitboard bishops = side->bishops | side->queens;
    Bitboard rooks = side->rooks | side->queens;

    Bitboard moves_bitboard = LNX_BITBOARD_EMPTY;

    while(kings)
    {
        Square from = LNX_BIT_LSB_INDEX(kings);
        LNX_BIT_LSB_RESET(kings);

        moves_bitboard = ~side->occupancy & king_attacks[from];

        Move move = LNX_MOVE_NONE;
        LNX_MOVE_SET_FROM(move, from);
        LNX_MOVE_SET_TYPE(move, LNX_MOVE_TYPE_NORMAL);

        while (moves_bitboard)
        {
            Square to = LNX_BIT_LSB_INDEX(moves_bitboard);

            if(position_attack_count_on_square_without_enemy_king(pos, !pos->side_to_move, to) == 0)
            {
                LNX_MOVE_CLEAR_TO(move);
                LNX_MOVE_SET_TO(move, to);
                move_list_add_move(move_list, move);
            }

            LNX_BIT_LSB_RESET(moves_bitboard);
        }

        if(pos->side_to_move == LNX_SIDE_WHITE)
        {
            uint8_t kingside_castling_squares_attack_count = position_attack_count_on_square(pos, !pos->side_to_move, LNX_SQUARE_E1) + position_attack_count_on_square(pos, !pos->side_to_move, LNX_SQUARE_F1) + position_attack_count_on_square(pos, !pos->side_to_move, LNX_SQUARE_G1);
            uint8_t queenside_castling_squares_attack_count = position_attack_count_on_square(pos, !pos->side_to_move, LNX_SQUARE_E1) + position_attack_count_on_square(pos, !pos->side_to_move, LNX_SQUARE_D1) + position_attack_count_on_square(pos, !pos->side_to_move, LNX_SQUARE_C1);

            if(pos->castling_perms & LNX_CASTLING_PERM_WHITE_KINGSIDE && (~pos->occupancy & white_king_kingside_castling_bitboard) == white_king_kingside_castling_bitboard && kingside_castling_squares_attack_count == 0)
            {
                LNX_MOVE_CLEAR_TO(move);
                LNX_MOVE_SET_TO(move, LNX_SQUARE_G1);

                LNX_MOVE_CLEAR_TYPE(move);
                LNX_MOVE_SET_TYPE(move, LNX_MOVE_TYPE_CASTLING);

                move_list_add_move(move_list, move);
            }

            if(pos->castling_perms & LNX_CASTLING_PERM_WHITE_QUEENSIDE && (~pos->occupancy & white_king_queenside_castling_bitboard) == white_king_queenside_castling_bitboard && queenside_castling_squares_attack_count == 0)
            {
                LNX_MOVE_CLEAR_TO(move);
                LNX_MOVE_SET_TO(move, LNX_SQUARE_C1);

                LNX_MOVE_CLEAR_TYPE(move);
                LNX_MOVE_SET_TYPE(move, LNX_MOVE_TYPE_CASTLING);

                move_list_add_move(move_list, move);
            }
        }
        else
        {
            uint8_t kingside_castling_squares_attack_count = position_attack_count_on_square(pos, !pos->side_to_move, LNX_SQUARE_E8) + position_attack_count_on_square(pos, !pos->side_to_move, LNX_SQUARE_F8) + position_attack_count_on_square(pos, !pos->side_to_move, LNX_SQUARE_G8);
            uint8_t queenside_castling_squares_attack_count = position_attack_count_on_square(pos, !pos->side_to_move, LNX_SQUARE_E8) + position_attack_count_on_square(pos, !pos->side_to_move, LNX_SQUARE_D8) + position_attack_count_on_square(pos, !pos->side_to_move, LNX_SQUARE_C8);

            if(pos->castling_perms & LNX_CASTLING_PERM_BLACK_KINGSIDE && (~pos->occupancy & black_king_kingside_castling_bitboard) == black_king_kingside_castling_bitboard && kingside_castling_squares_attack_count == 0)
            {
                LNX_MOVE_CLEAR_TO(move);
                LNX_MOVE_SET_TO(move, LNX_SQUARE_G8);

                LNX_MOVE_CLEAR_TYPE(move);
                LNX_MOVE_SET_TYPE(move, LNX_MOVE_TYPE_CASTLING);

                move_list_add_move(move_list, move);
            }

            if(pos->castling_perms & LNX_CASTLING_PERM_BLACK_QUEENSIDE && (~pos->occupancy & black_king_queenside_castling_bitboard) == black_king_queenside_castling_bitboard && queenside_castling_squares_attack_count == 0)
            {
                LNX_MOVE_CLEAR_TO(move);
                LNX_MOVE_SET_TO(move, LNX_SQUARE_C8);

                LNX_MOVE_CLEAR_TYPE(move);
                LNX_MOVE_SET_TYPE(move, LNX_MOVE_TYPE_CASTLING);

                move_list_add_move(move_list, move);
            }
        }
    }

    if(position_attack_count_on_square(pos, !pos->side_to_move, LNX_BIT_LSB_INDEX(side->kings)) > 1)
        return;

    while(pawns)
    {
        Square from = LNX_BIT_LSB_INDEX(pawns);
        Bitboard from_bitboard = LNX_BIT(from);
        LNX_BIT_LSB_RESET(pawns);

        moves_bitboard = pos->side_to_move == LNX_SIDE_WHITE ? (white_pawn_attacks[from] & pos->sides[LNX_SIDE_BLACK].occupancy) : (black_pawn_attacks[from] & pos->sides[LNX_SIDE_WHITE].occupancy);

        Bitboard pawn_push_bitboard = LNX_BITBOARD_EMPTY;
        if(pos->side_to_move == LNX_SIDE_WHITE)
        {
            pawn_push_bitboard = ~pos->occupancy & white_pawn_pushs[from];
            pawn_push_bitboard &= ~((pos->occupancy & ~from_bitboard) << LNX_BOARD_WIDTH);
        }
        else
        {
            pawn_push_bitboard = ~pos->occupancy & black_pawn_pushs[from];
            pawn_push_bitboard &= ~((pos->occupancy & ~from_bitboard) >> LNX_BOARD_WIDTH);
        }

        moves_bitboard = (moves_bitboard | pawn_push_bitboard) & check_mask;

        if(from_bitboard & pin_mask_horizontal_vertical)
            moves_bitboard &= pin_mask_horizontal_vertical;

        if(from_bitboard & pin_mask_diagonal)
            moves_bitboard &= pin_mask_diagonal;

        Move move = LNX_MOVE_NONE;
        LNX_MOVE_SET_FROM(move, from);
        LNX_MOVE_SET_TYPE(move, LNX_MOVE_TYPE_NORMAL);

        while (moves_bitboard)
        {
            Square to = LNX_BIT_LSB_INDEX(moves_bitboard);
            Rank to_rank = LNX_SQUARE_TO_RANK(to);

            LNX_MOVE_CLEAR_TO(move);
            LNX_MOVE_SET_TO(move, to);

            if(to_rank == LNX_RANK_1 || to_rank == LNX_RANK_8)
            {
                LNX_MOVE_CLEAR_TYPE(move);
                LNX_MOVE_SET_TYPE(move, LNX_MOVE_TYPE_PROMOTION);

                for(PromotionPieceType promotion_piece = LNX_PROMOTION_PIECE_TYPE_KNIGHT; promotion_piece <= LNX_PROMOTION_PIECE_TYPE_QUEEN; ++promotion_piece)
                {
                    LNX_MOVE_CLEAR_PROMOTION_PIECE(move);
                    LNX_MOVE_SET_PROMOTION_PIECE(move, promotion_piece);
                    move_list_add_move(move_list, move);
                }
            }
            else
                move_list_add_move(move_list, move);

            LNX_BIT_LSB_RESET(moves_bitboard);
        }

        Bitboard pawn_attacks = pos->side_to_move == LNX_SIDE_WHITE ? white_pawn_attacks[from] : black_pawn_attacks[from];
        Square captured_pawn_square = pos->side_to_move == LNX_SIDE_WHITE ? (pos->ep_square - LNX_BOARD_WIDTH) : (pos->ep_square + LNX_BOARD_WIDTH);
        Bitboard captured_pawn_bitboard = LNX_BIT(captured_pawn_square);
        if(pos->ep_square != LNX_SQUARE_OFFBOARD && pawn_attacks & LNX_BIT(pos->ep_square) && (pawn_attacks | captured_pawn_bitboard) & check_mask)
        {
            const Side* enemy = &pos->sides[!pos->side_to_move];
            Bitboard enemy_bishops = enemy->bishops | enemy->queens;
            Bitboard enemy_rooks = enemy->rooks | enemy->queens;

            Square king_square = LNX_BIT_LSB_INDEX(side->kings);
            LunoxBool captured_pawn_pinned = LNX_FALSE;

            while(enemy_bishops)
            {
                Square enemy_bishop = LNX_BIT_LSB_INDEX(enemy_bishops);

                Bitboard bishop_attacks = bitboard_get_bishop_attacks(enemy_bishop, side->kings);
                Bitboard ray = bishop_attacks & bitboard_get_bishop_attacks(king_square, LNX_BIT(enemy_bishop));

                if(bishop_attacks & side->kings && ray & captured_pawn_bitboard && LNX_BIT_COUNT((ray & pos->occupancy) & ~captured_pawn_bitboard) == 0)
                {
                    captured_pawn_pinned = LNX_TRUE;
                    break;
                }

                LNX_BIT_LSB_RESET(enemy_bishops);
            }

            while(enemy_rooks)
            {
                Square enemy_rook = LNX_BIT_LSB_INDEX(enemy_rooks);

                Bitboard rock_attacks = bitboard_get_rook_attacks(enemy_rook, side->kings);
                Bitboard ray = rock_attacks & bitboard_get_rook_attacks(king_square, LNX_BIT(enemy_rook));

                if(rock_attacks & side->kings && ray & captured_pawn_bitboard && LNX_BIT_COUNT((ray & pos->occupancy) & ~from_bitboard) == 1)
                {
                    captured_pawn_pinned = LNX_TRUE;
                    break;
                }

                LNX_BIT_LSB_RESET(enemy_rooks);
            }

            if(!captured_pawn_pinned && from_bitboard & ~pin_mask_horizontal_vertical)
            {
                if(from_bitboard & pin_mask_diagonal)
                {
                    if(LNX_BIT(pos->ep_square) & pin_mask_diagonal)
                    {
                        LNX_MOVE_CLEAR_TO(move);
                        LNX_MOVE_SET_TO(move, pos->ep_square);

                        LNX_MOVE_CLEAR_TYPE(move);
                        LNX_MOVE_SET_TYPE(move, LNX_MOVE_TYPE_EN_PASSANT);
                        move_list_add_move(move_list, move);
                    }
                }
                else
                {
                    LNX_MOVE_CLEAR_TO(move);
                    LNX_MOVE_SET_TO(move, pos->ep_square);

                    LNX_MOVE_CLEAR_TYPE(move);
                    LNX_MOVE_SET_TYPE(move, LNX_MOVE_TYPE_EN_PASSANT);
                    move_list_add_move(move_list, move);
                }
            }
        }
    }

    knights &= ~pin_mask;
    while(knights)
    {
        Square from = LNX_BIT_LSB_INDEX(knights);
        LNX_BIT_LSB_RESET(knights);

        moves_bitboard = (~side->occupancy & knight_attacks[from]) & check_mask;

        Move move = LNX_MOVE_NONE;
        LNX_MOVE_SET_FROM(move, from);
        LNX_MOVE_SET_TYPE(move, LNX_MOVE_TYPE_NORMAL);

        while (moves_bitboard)
        {
            Square to = LNX_BIT_LSB_INDEX(moves_bitboard);

            LNX_MOVE_CLEAR_TO(move);
            LNX_MOVE_SET_TO(move, to);
            move_list_add_move(move_list, move);

            LNX_BIT_LSB_RESET(moves_bitboard);
        }
    }

    bishops &= ~pin_mask_horizontal_vertical;
    while(bishops)
    {
        Square from = LNX_BIT_LSB_INDEX(bishops);
        Bitboard from_bitboard = LNX_BIT(from);
        LNX_BIT_LSB_RESET(bishops);

        moves_bitboard = (~side->occupancy & bitboard_get_bishop_attacks(from, pos->occupancy)) & check_mask;

        if(from_bitboard & pin_mask_diagonal)
            moves_bitboard &= pin_mask_diagonal;

        Move move = LNX_MOVE_NONE;
        LNX_MOVE_SET_FROM(move, from);
        LNX_MOVE_SET_TYPE(move, LNX_MOVE_TYPE_NORMAL);

        while (moves_bitboard)
        {
            Square to = LNX_BIT_LSB_INDEX(moves_bitboard);

            LNX_MOVE_CLEAR_TO(move);
            LNX_MOVE_SET_TO(move, to);
            move_list_add_move(move_list, move);

            LNX_BIT_LSB_RESET(moves_bitboard);
        }
    }

    rooks &= ~pin_mask_diagonal;
    while(rooks)
    {
        Square from = LNX_BIT_LSB_INDEX(rooks);
        Bitboard from_bitboard = LNX_BIT(from);
        LNX_BIT_LSB_RESET(rooks);

        moves_bitboard = (~side->occupancy & bitboard_get_rook_attacks(from, pos->occupancy)) & check_mask;

        if(from_bitboard & pin_mask_horizontal_vertical)
            moves_bitboard &= pin_mask_horizontal_vertical;

        Move move = LNX_MOVE_NONE;
        LNX_MOVE_SET_FROM(move, from);
        LNX_MOVE_SET_TYPE(move, LNX_MOVE_TYPE_NORMAL);

        while (moves_bitboard)
        {
            Square to = LNX_BIT_LSB_INDEX(moves_bitboard);

            LNX_MOVE_CLEAR_TO(move);
            LNX_MOVE_SET_TO(move, to);
            move_list_add_move(move_list, move);

            LNX_BIT_LSB_RESET(moves_bitboard);
        }
    }
}