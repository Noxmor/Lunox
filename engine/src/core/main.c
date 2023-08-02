#include <stdio.h>
#include <stdlib.h>

#include <lunox/boards/position.h>
#include <lunox/boards/board.h>

int main(void)
{
    Position pos;
    position_startpos(&pos);
    position_print(&pos, LNX_SIDE_WHITE);

    char line[128];
    Move move;

    while(LNX_TRUE)
    {
        move = LNX_MOVE_NONE;

        fgets(line, sizeof(line), stdin);

        if(line[0] == 'q')
            break;

        uint8_t from = LNX_FILE_RANK_TO_SQUARE(line[0] - 'a', line[1] - '1');
        uint8_t to = LNX_FILE_RANK_TO_SQUARE(line[2] - 'a', line[3] - '1');

        LNX_MOVE_SET_FROM(move, from);
        LNX_MOVE_SET_TO(move, to);
        LNX_MOVE_SET_TYPE(move, LNX_MOVE_TYPE_NORMAL);

        position_make_move(&pos, move);
        position_print(&pos, LNX_SIDE_WHITE);
    }

    return EXIT_SUCCESS;
}