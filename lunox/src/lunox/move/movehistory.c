#include "movehistory.h"

void move_history_push(MoveHistory* move_history, MoveHistoryEntry entry);
{
    LNX_ASSERT(move_history != NULL);

    move_history->entries[move_history->entries_count++] = entry;
}

MoveHistoryEntry move_history_pop(MoveHistory* move_history)
{
    LNX_ASSERT(move_history != NULL);

    MoveHistoryEntry entry = move_history_peek(move_history);
    --move_history->entries_count;
    
    return entry;
}

MoveHistoryEntry move_history_peek(const MoveHistory* move_history)
{
    LNX_ASSERT(move_history != NULL);

    LNX_VERIFY(move_history->entries_count > 0);

    return move_history->entries[move_history->entries_count - 1];
}

void move_history_reset(MoveHistory* move_history)
{
    LNX_ASSERT(move_history != NULL);

    move_history->entries_count = 0;
}