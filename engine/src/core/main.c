#include <stdio.h>
#include <stdlib.h>

#include <lunox/boards/position.h>
#include <lunox/move/movegen.h>
#include <lunox/boards/board.h>

int main(int argc, char** argv)
{
    bitboard_init();

    if(argc >= 2)
    {
        if(!bitboard_init_magics(argv[1]))
        {
            printf("Invalid magic numbers provided!\n");
            return EXIT_FAILURE;
        }
    }
    else
        bitboard_init_default_magics();

    Position pos;
    position_startpos(&pos);
    position_print(&pos, LNX_SIDE_WHITE);

    MoveList move_list;

    char line[128];

    while(LNX_TRUE)
    {
        movegen_generate_moves(&pos, &move_list);

        for(uint8_t i = 0; i < move_list.moves_count; ++i)
        {
            Move move = move_list.moves[i];
            Square from = LNX_MOVE_GET_FROM(move);
            Square to = LNX_MOVE_GET_TO(move);
            char promotion_piece = ' ';
            if(LNX_MOVE_GET_TYPE(move) == LNX_MOVE_TYPE_PROMOTION)
            {
                switch(LNX_MOVE_GET_PROMOTION_PIECE(move))
                {
                    case LNX_PROMOTION_PIECE_TYPE_KNIGHT: promotion_piece = 'n'; break;
                    case LNX_PROMOTION_PIECE_TYPE_BISHOP: promotion_piece = 'b'; break;
                    case LNX_PROMOTION_PIECE_TYPE_ROOK: promotion_piece = 'r'; break;
                    case LNX_PROMOTION_PIECE_TYPE_QUEEN: promotion_piece = 'q'; break;
                }

                printf("%d. %c%c%c%c%c\n", i + 1, LNX_SQUARE_TO_FILE(from) + 'a', LNX_SQUARE_TO_RANK(from) + '1', LNX_SQUARE_TO_FILE(to) + 'a', LNX_SQUARE_TO_RANK(to) + '1', promotion_piece);
            }
            else
                printf("%d. %c%c%c%c\n", i + 1, LNX_SQUARE_TO_FILE(from) + 'a', LNX_SQUARE_TO_RANK(from) + '1', LNX_SQUARE_TO_FILE(to) + 'a', LNX_SQUARE_TO_RANK(to) + '1');
        }

        fgets(line, sizeof(line), stdin);

        if(line[0] == 'q')
            break;

        Square from = LNX_FILE_RANK_TO_SQUARE(line[0] - 'a', line[1] - '1');
        Square to = LNX_FILE_RANK_TO_SQUARE(line[2] - 'a', line[3] - '1');
        PromotionPieceType promotion_piece = 0;
        switch(line[4])
        {
            case 'k': promotion_piece = LNX_PROMOTION_PIECE_TYPE_KNIGHT; break;
            case 'b': promotion_piece = LNX_PROMOTION_PIECE_TYPE_BISHOP; break;
            case 'r': promotion_piece = LNX_PROMOTION_PIECE_TYPE_ROOK; break;
            case 'q': promotion_piece = LNX_PROMOTION_PIECE_TYPE_QUEEN; break;
        }

        for(uint8_t i = 0; i < move_list.moves_count; ++i)
        {
            Move move = move_list.moves[i];

            if(from == LNX_MOVE_GET_FROM(move) && to == LNX_MOVE_GET_TO(move))
            {
                uint8_t type = LNX_MOVE_GET_TYPE(move);

                if(type != LNX_MOVE_TYPE_PROMOTION)
                {
                    position_make_move(&pos, move);
                    break;
                }

                if(promotion_piece == LNX_MOVE_GET_PROMOTION_PIECE(move))
                {
                    position_make_move(&pos, move);
                    break;
                }
            }
        }

        position_print(&pos, LNX_SIDE_WHITE);
    }

    return EXIT_SUCCESS;
}