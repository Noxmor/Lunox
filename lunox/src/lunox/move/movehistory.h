#ifndef LNX_MOVE_HISTORY_H
#define LNX_MOVE_HISTORY_H

#include <lunox/boards/bitboard.h>

#include <lunox/move/move.h>

typedef struct MoveHistoryEntry
{
    Move move;
    uint8_t castling_perms;
    uint8_t fifty_move_rule;
    PieceType captured_piece_type;
} MoveHistoryEntry;

#define LNX_MOVE_HISTORY_CAPACITY 5988

typedef struct MoveHistory
{
    MoveHistoryEntry entries[LNX_MOVE_HISTORY_CAPACITY];
    uint16_t entries_count;

} MoveHistory;

void move_history_push(MoveHistory* move_history, MoveHistoryEntry entry);

MoveHistoryEntry move_history_pop(MoveHistory* move_history);

MoveHistoryEntry move_history_peek(const MoveHistory* move_history);

void move_history_reset(MoveHistory* move_history);

#endif