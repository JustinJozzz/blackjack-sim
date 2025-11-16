#include "strategy.h"
#include "card.h"

// Helper macros for readability
#define DEALER_IDX(card_value) ((card_value) - 2)  // Maps dealer 2-11 to indices 0-9
#define HARD_IDX(hand_value) ((hand_value) - 8)     // Maps player 8-16 to indices 0-8
#define SOFT_IDX(hand_value) ((hand_value) - 13)    // Maps A-2(13) through A-9(20) to indices 0-7

// Pair indices: 0=2-2, 1=3-3, ..., 7=9-9, 8=T-T, 9=A-A
#define PAIR_IDX_2  0
#define PAIR_IDX_3  1
#define PAIR_IDX_4  2
#define PAIR_IDX_5  3
#define PAIR_IDX_6  4
#define PAIR_IDX_7  5
#define PAIR_IDX_8  6
#define PAIR_IDX_9  7
#define PAIR_IDX_T  8
#define PAIR_IDX_A  9

// Surrender indices: 0=14, 1=15, 2=16
#define SUR_IDX_14  0
#define SUR_IDX_15  1
#define SUR_IDX_16  2

void basic_strategy_init(BasicStrategy* strategy) {
    // =======================================================================
    // HARD TOTALS (player 8-16 vs dealer 2-11)
    // =======================================================================
    // Reading from the chart bottom to top (8, 9, 10, 11, 12, 13, 14, 15, 16)

    // Dealer: 2, 3, 4, 5, 6, 7, 8, 9, 10, A
    // Player 8: H, H, H, H, H, H, H, H, H, H
    for (int d = 0; d < 10; d++) {
        strategy->hard_totals[d][HARD_IDX(8)] = HIT;
    }

    // Player 9: H, D, D, D, D, H, H, H, H, H
    strategy->hard_totals[DEALER_IDX(2)][HARD_IDX(9)] = HIT;
    strategy->hard_totals[DEALER_IDX(3)][HARD_IDX(9)] = DOUBLE;
    strategy->hard_totals[DEALER_IDX(4)][HARD_IDX(9)] = DOUBLE;
    strategy->hard_totals[DEALER_IDX(5)][HARD_IDX(9)] = DOUBLE;
    strategy->hard_totals[DEALER_IDX(6)][HARD_IDX(9)] = DOUBLE;
    strategy->hard_totals[DEALER_IDX(7)][HARD_IDX(9)] = HIT;
    strategy->hard_totals[DEALER_IDX(8)][HARD_IDX(9)] = HIT;
    strategy->hard_totals[DEALER_IDX(9)][HARD_IDX(9)] = HIT;
    strategy->hard_totals[DEALER_IDX(10)][HARD_IDX(9)] = HIT;
    strategy->hard_totals[DEALER_IDX(11)][HARD_IDX(9)] = HIT;

    // Player 10: D, D, D, D, D, D, D, D, H, H
    for (int d = 0; d < 8; d++) {
        strategy->hard_totals[d][HARD_IDX(10)] = DOUBLE;
    }
    strategy->hard_totals[DEALER_IDX(10)][HARD_IDX(10)] = HIT;
    strategy->hard_totals[DEALER_IDX(11)][HARD_IDX(10)] = HIT;

    // Player 11: D, D, D, D, D, D, D, D, D, D
    for (int d = 0; d < 10; d++) {
        strategy->hard_totals[d][HARD_IDX(11)] = DOUBLE;
    }

    // Player 12: H, H, S, S, S, H, H, H, H, H
    strategy->hard_totals[DEALER_IDX(2)][HARD_IDX(12)] = HIT;
    strategy->hard_totals[DEALER_IDX(3)][HARD_IDX(12)] = HIT;
    strategy->hard_totals[DEALER_IDX(4)][HARD_IDX(12)] = STAND;
    strategy->hard_totals[DEALER_IDX(5)][HARD_IDX(12)] = STAND;
    strategy->hard_totals[DEALER_IDX(6)][HARD_IDX(12)] = STAND;
    strategy->hard_totals[DEALER_IDX(7)][HARD_IDX(12)] = HIT;
    strategy->hard_totals[DEALER_IDX(8)][HARD_IDX(12)] = HIT;
    strategy->hard_totals[DEALER_IDX(9)][HARD_IDX(12)] = HIT;
    strategy->hard_totals[DEALER_IDX(10)][HARD_IDX(12)] = HIT;
    strategy->hard_totals[DEALER_IDX(11)][HARD_IDX(12)] = HIT;

    // Player 13: S, S, S, S, S, H, H, H, H, H
    for (int d = 0; d < 5; d++) {
        strategy->hard_totals[d][HARD_IDX(13)] = STAND;
    }
    for (int d = 5; d < 10; d++) {
        strategy->hard_totals[d][HARD_IDX(13)] = HIT;
    }

    // Player 14: S, S, S, S, S, H, H, H, H, H (same as 13)
    for (int d = 0; d < 5; d++) {
        strategy->hard_totals[d][HARD_IDX(14)] = STAND;
    }
    for (int d = 5; d < 10; d++) {
        strategy->hard_totals[d][HARD_IDX(14)] = HIT;
    }

    // Player 15: S, S, S, S, S, H, H, H, H, H (same as 13, 14)
    for (int d = 0; d < 5; d++) {
        strategy->hard_totals[d][HARD_IDX(15)] = STAND;
    }
    for (int d = 5; d < 10; d++) {
        strategy->hard_totals[d][HARD_IDX(15)] = HIT;
    }

    // Player 16: S, S, S, S, S, H, H, H, H, H (same as 13-15)
    for (int d = 0; d < 5; d++) {
        strategy->hard_totals[d][HARD_IDX(16)] = STAND;
    }
    for (int d = 5; d < 10; d++) {
        strategy->hard_totals[d][HARD_IDX(16)] = HIT;
    }

    // =======================================================================
    // SOFT TOTALS (A-2 through A-9 vs dealer 2-11)
    // =======================================================================
    // Reading from chart bottom to top: A-2(13), A-3(14), A-4(15), A-5(16), A-6(17), A-7(18), A-8(19), A-9(20)

    // A-2 (13): H, H, H, D, D, H, H, H, H, H
    strategy->soft_totals[DEALER_IDX(2)][SOFT_IDX(13)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(3)][SOFT_IDX(13)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(4)][SOFT_IDX(13)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(5)][SOFT_IDX(13)] = SOFT_DOUBLE_OR_HIT;
    strategy->soft_totals[DEALER_IDX(6)][SOFT_IDX(13)] = SOFT_DOUBLE_OR_HIT;
    strategy->soft_totals[DEALER_IDX(7)][SOFT_IDX(13)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(8)][SOFT_IDX(13)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(9)][SOFT_IDX(13)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(10)][SOFT_IDX(13)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(11)][SOFT_IDX(13)] = SOFT_HIT;

    // A-3 (14): H, H, H, D, D, H, H, H, H, H (same as A-2)
    strategy->soft_totals[DEALER_IDX(2)][SOFT_IDX(14)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(3)][SOFT_IDX(14)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(4)][SOFT_IDX(14)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(5)][SOFT_IDX(14)] = SOFT_DOUBLE_OR_HIT;
    strategy->soft_totals[DEALER_IDX(6)][SOFT_IDX(14)] = SOFT_DOUBLE_OR_HIT;
    strategy->soft_totals[DEALER_IDX(7)][SOFT_IDX(14)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(8)][SOFT_IDX(14)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(9)][SOFT_IDX(14)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(10)][SOFT_IDX(14)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(11)][SOFT_IDX(14)] = SOFT_HIT;

    // A-4 (15): H, H, D, D, D, H, H, H, H, H
    strategy->soft_totals[DEALER_IDX(2)][SOFT_IDX(15)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(3)][SOFT_IDX(15)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(4)][SOFT_IDX(15)] = SOFT_DOUBLE_OR_HIT;
    strategy->soft_totals[DEALER_IDX(5)][SOFT_IDX(15)] = SOFT_DOUBLE_OR_HIT;
    strategy->soft_totals[DEALER_IDX(6)][SOFT_IDX(15)] = SOFT_DOUBLE_OR_HIT;
    strategy->soft_totals[DEALER_IDX(7)][SOFT_IDX(15)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(8)][SOFT_IDX(15)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(9)][SOFT_IDX(15)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(10)][SOFT_IDX(15)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(11)][SOFT_IDX(15)] = SOFT_HIT;

    // A-5 (16): H, H, D, D, D, H, H, H, H, H (same as A-4)
    strategy->soft_totals[DEALER_IDX(2)][SOFT_IDX(16)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(3)][SOFT_IDX(16)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(4)][SOFT_IDX(16)] = SOFT_DOUBLE_OR_HIT;
    strategy->soft_totals[DEALER_IDX(5)][SOFT_IDX(16)] = SOFT_DOUBLE_OR_HIT;
    strategy->soft_totals[DEALER_IDX(6)][SOFT_IDX(16)] = SOFT_DOUBLE_OR_HIT;
    strategy->soft_totals[DEALER_IDX(7)][SOFT_IDX(16)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(8)][SOFT_IDX(16)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(9)][SOFT_IDX(16)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(10)][SOFT_IDX(16)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(11)][SOFT_IDX(16)] = SOFT_HIT;

    // A-6 (17): H, D, D, D, D, H, H, H, H, H
    strategy->soft_totals[DEALER_IDX(2)][SOFT_IDX(17)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(3)][SOFT_IDX(17)] = SOFT_DOUBLE_OR_HIT;
    strategy->soft_totals[DEALER_IDX(4)][SOFT_IDX(17)] = SOFT_DOUBLE_OR_HIT;
    strategy->soft_totals[DEALER_IDX(5)][SOFT_IDX(17)] = SOFT_DOUBLE_OR_HIT;
    strategy->soft_totals[DEALER_IDX(6)][SOFT_IDX(17)] = SOFT_DOUBLE_OR_HIT;
    strategy->soft_totals[DEALER_IDX(7)][SOFT_IDX(17)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(8)][SOFT_IDX(17)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(9)][SOFT_IDX(17)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(10)][SOFT_IDX(17)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(11)][SOFT_IDX(17)] = SOFT_HIT;

    // A-7 (18): Ds, Ds, Ds, Ds, Ds, S, S, H, H, H
    strategy->soft_totals[DEALER_IDX(2)][SOFT_IDX(18)] = SOFT_DOUBLE_OR_STAND;
    strategy->soft_totals[DEALER_IDX(3)][SOFT_IDX(18)] = SOFT_DOUBLE_OR_STAND;
    strategy->soft_totals[DEALER_IDX(4)][SOFT_IDX(18)] = SOFT_DOUBLE_OR_STAND;
    strategy->soft_totals[DEALER_IDX(5)][SOFT_IDX(18)] = SOFT_DOUBLE_OR_STAND;
    strategy->soft_totals[DEALER_IDX(6)][SOFT_IDX(18)] = SOFT_DOUBLE_OR_STAND;
    strategy->soft_totals[DEALER_IDX(7)][SOFT_IDX(18)] = SOFT_STAND;
    strategy->soft_totals[DEALER_IDX(8)][SOFT_IDX(18)] = SOFT_STAND;
    strategy->soft_totals[DEALER_IDX(9)][SOFT_IDX(18)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(10)][SOFT_IDX(18)] = SOFT_HIT;
    strategy->soft_totals[DEALER_IDX(11)][SOFT_IDX(18)] = SOFT_HIT;

    // A-8 (19): S, S, S, S, Ds, S, S, S, S, S
    strategy->soft_totals[DEALER_IDX(2)][SOFT_IDX(19)] = SOFT_STAND;
    strategy->soft_totals[DEALER_IDX(3)][SOFT_IDX(19)] = SOFT_STAND;
    strategy->soft_totals[DEALER_IDX(4)][SOFT_IDX(19)] = SOFT_STAND;
    strategy->soft_totals[DEALER_IDX(5)][SOFT_IDX(19)] = SOFT_STAND;
    strategy->soft_totals[DEALER_IDX(6)][SOFT_IDX(19)] = SOFT_DOUBLE_OR_STAND;
    strategy->soft_totals[DEALER_IDX(7)][SOFT_IDX(19)] = SOFT_STAND;
    strategy->soft_totals[DEALER_IDX(8)][SOFT_IDX(19)] = SOFT_STAND;
    strategy->soft_totals[DEALER_IDX(9)][SOFT_IDX(19)] = SOFT_STAND;
    strategy->soft_totals[DEALER_IDX(10)][SOFT_IDX(19)] = SOFT_STAND;
    strategy->soft_totals[DEALER_IDX(11)][SOFT_IDX(19)] = SOFT_STAND;

    // A-9 (20): S, S, S, S, S, S, S, S, S, S
    for (int d = 0; d < 10; d++) {
        strategy->soft_totals[d][SOFT_IDX(20)] = SOFT_STAND;
    }

    // =======================================================================
    // PAIR SPLITTING (pairs vs dealer 2-11)
    // =======================================================================

    // 2-2: Y/N, Y/N, Y, Y, Y, Y, N, N, N, N
    strategy->pairs[DEALER_IDX(2)][PAIR_IDX_2] = YES_IF_SPLIT_OFFERED;
    strategy->pairs[DEALER_IDX(3)][PAIR_IDX_2] = YES_IF_SPLIT_OFFERED;
    strategy->pairs[DEALER_IDX(4)][PAIR_IDX_2] = YES;
    strategy->pairs[DEALER_IDX(5)][PAIR_IDX_2] = YES;
    strategy->pairs[DEALER_IDX(6)][PAIR_IDX_2] = YES;
    strategy->pairs[DEALER_IDX(7)][PAIR_IDX_2] = YES;
    strategy->pairs[DEALER_IDX(8)][PAIR_IDX_2] = NO;
    strategy->pairs[DEALER_IDX(9)][PAIR_IDX_2] = NO;
    strategy->pairs[DEALER_IDX(10)][PAIR_IDX_2] = NO;
    strategy->pairs[DEALER_IDX(11)][PAIR_IDX_2] = NO;

    // 3-3: Y/N, Y/N, Y, Y, Y, Y, N, N, N, N (same as 2-2)
    strategy->pairs[DEALER_IDX(2)][PAIR_IDX_3] = YES_IF_SPLIT_OFFERED;
    strategy->pairs[DEALER_IDX(3)][PAIR_IDX_3] = YES_IF_SPLIT_OFFERED;
    strategy->pairs[DEALER_IDX(4)][PAIR_IDX_3] = YES;
    strategy->pairs[DEALER_IDX(5)][PAIR_IDX_3] = YES;
    strategy->pairs[DEALER_IDX(6)][PAIR_IDX_3] = YES;
    strategy->pairs[DEALER_IDX(7)][PAIR_IDX_3] = YES;
    strategy->pairs[DEALER_IDX(8)][PAIR_IDX_3] = NO;
    strategy->pairs[DEALER_IDX(9)][PAIR_IDX_3] = NO;
    strategy->pairs[DEALER_IDX(10)][PAIR_IDX_3] = NO;
    strategy->pairs[DEALER_IDX(11)][PAIR_IDX_3] = NO;

    // 4-4: N, N, N, Y/N, Y/N, N, N, N, N, N
    strategy->pairs[DEALER_IDX(2)][PAIR_IDX_4] = NO;
    strategy->pairs[DEALER_IDX(3)][PAIR_IDX_4] = NO;
    strategy->pairs[DEALER_IDX(4)][PAIR_IDX_4] = NO;
    strategy->pairs[DEALER_IDX(5)][PAIR_IDX_4] = YES_IF_SPLIT_OFFERED;
    strategy->pairs[DEALER_IDX(6)][PAIR_IDX_4] = YES_IF_SPLIT_OFFERED;
    strategy->pairs[DEALER_IDX(7)][PAIR_IDX_4] = NO;
    strategy->pairs[DEALER_IDX(8)][PAIR_IDX_4] = NO;
    strategy->pairs[DEALER_IDX(9)][PAIR_IDX_4] = NO;
    strategy->pairs[DEALER_IDX(10)][PAIR_IDX_4] = NO;
    strategy->pairs[DEALER_IDX(11)][PAIR_IDX_4] = NO;

    // 5-5: N, N, N, N, N, N, N, N, N, N (never split 5s)
    for (int d = 0; d < 10; d++) {
        strategy->pairs[d][PAIR_IDX_5] = NO;
    }

    // 6-6: Y/N, Y/N, Y/N, Y/N, Y/N, N, N, N, N, N
    strategy->pairs[DEALER_IDX(2)][PAIR_IDX_6] = YES_IF_SPLIT_OFFERED;
    strategy->pairs[DEALER_IDX(3)][PAIR_IDX_6] = YES_IF_SPLIT_OFFERED;
    strategy->pairs[DEALER_IDX(4)][PAIR_IDX_6] = YES_IF_SPLIT_OFFERED;
    strategy->pairs[DEALER_IDX(5)][PAIR_IDX_6] = YES_IF_SPLIT_OFFERED;
    strategy->pairs[DEALER_IDX(6)][PAIR_IDX_6] = YES_IF_SPLIT_OFFERED;
    strategy->pairs[DEALER_IDX(7)][PAIR_IDX_6] = NO;
    strategy->pairs[DEALER_IDX(8)][PAIR_IDX_6] = NO;
    strategy->pairs[DEALER_IDX(9)][PAIR_IDX_6] = NO;
    strategy->pairs[DEALER_IDX(10)][PAIR_IDX_6] = NO;
    strategy->pairs[DEALER_IDX(11)][PAIR_IDX_6] = NO;

    // 7-7: Y, Y, Y, Y, Y, Y, N, N, N, N
    strategy->pairs[DEALER_IDX(2)][PAIR_IDX_7] = YES;
    strategy->pairs[DEALER_IDX(3)][PAIR_IDX_7] = YES;
    strategy->pairs[DEALER_IDX(4)][PAIR_IDX_7] = YES;
    strategy->pairs[DEALER_IDX(5)][PAIR_IDX_7] = YES;
    strategy->pairs[DEALER_IDX(6)][PAIR_IDX_7] = YES;
    strategy->pairs[DEALER_IDX(7)][PAIR_IDX_7] = YES;
    strategy->pairs[DEALER_IDX(8)][PAIR_IDX_7] = NO;
    strategy->pairs[DEALER_IDX(9)][PAIR_IDX_7] = NO;
    strategy->pairs[DEALER_IDX(10)][PAIR_IDX_7] = NO;
    strategy->pairs[DEALER_IDX(11)][PAIR_IDX_7] = NO;

    // 8-8: Y, Y, Y, Y, Y, Y, Y, Y, Y, Y (always split 8s)
    for (int d = 0; d < 10; d++) {
        strategy->pairs[d][PAIR_IDX_8] = YES;
    }

    // 9-9: Y, Y, Y, Y, Y, N, Y, Y, N, N
    strategy->pairs[DEALER_IDX(2)][PAIR_IDX_9] = YES;
    strategy->pairs[DEALER_IDX(3)][PAIR_IDX_9] = YES;
    strategy->pairs[DEALER_IDX(4)][PAIR_IDX_9] = YES;
    strategy->pairs[DEALER_IDX(5)][PAIR_IDX_9] = YES;
    strategy->pairs[DEALER_IDX(6)][PAIR_IDX_9] = YES;
    strategy->pairs[DEALER_IDX(7)][PAIR_IDX_9] = NO;
    strategy->pairs[DEALER_IDX(8)][PAIR_IDX_9] = YES;
    strategy->pairs[DEALER_IDX(9)][PAIR_IDX_9] = YES;
    strategy->pairs[DEALER_IDX(10)][PAIR_IDX_9] = NO;
    strategy->pairs[DEALER_IDX(11)][PAIR_IDX_9] = NO;

    // T-T: N, N, N, N, N, N, N, N, N, N (never split 10s)
    for (int d = 0; d < 10; d++) {
        strategy->pairs[d][PAIR_IDX_T] = NO;
    }

    // A-A: Y, Y, Y, Y, Y, Y, Y, Y, Y, Y (always split Aces)
    for (int d = 0; d < 10; d++) {
        strategy->pairs[d][PAIR_IDX_A] = YES;
    }

    // =======================================================================
    // LATE SURRENDER (player 14, 15, 16 vs dealer 2-11)
    // =======================================================================

    // Initialize all to false
    for (int d = 0; d < 10; d++) {
        strategy->surrender[d][SUR_IDX_14] = false;
        strategy->surrender[d][SUR_IDX_15] = false;
        strategy->surrender[d][SUR_IDX_16] = false;
    }

    // 16 vs 9, 10, A: surrender
    strategy->surrender[DEALER_IDX(9)][SUR_IDX_16] = true;
    strategy->surrender[DEALER_IDX(10)][SUR_IDX_16] = true;
    strategy->surrender[DEALER_IDX(11)][SUR_IDX_16] = true;

    // 15 vs 10: surrender
    strategy->surrender[DEALER_IDX(10)][SUR_IDX_15] = true;
}

PlayerAction get_basic_strategy_action(Hand* player_hand, int dealer_up_card, Rules* rules, BasicStrategy* strategy, bool can_split, bool can_double) {
    int player_hand_value = hand_get_value(player_hand);
    int dealer_up_card_value = card_value(dealer_up_card);
    int dealer_idx = DEALER_IDX(dealer_up_card_value);

    // 1. Check if hand is a pair and look up pairs table
    if (can_split && hand_can_split(player_hand)) {
        // For pairs, need to map card rank to pair index
        int rank = card_rank(player_hand->cards[0]);
        int pair_idx;
        if (rank == 0) {  // Ace
            pair_idx = PAIR_IDX_A;
        } else if (rank >= 9) {  // 10, J, Q, K all map to 10-value
            pair_idx = PAIR_IDX_T;
        } else {  // 2-9
            pair_idx = rank - 1;  // rank 1 (2) -> idx 0, rank 2 (3) -> idx 1, etc.
        }

        SplitAction action = strategy->pairs[dealer_idx][pair_idx];
        if (action == YES || (action == YES_IF_SPLIT_OFFERED && rules->double_after_split)) {
            return SPLIT;
        }
    }

    // 2. Check if hand is soft and look up soft_totals table
    if (hand_is_soft(player_hand)) {
        SoftHandPlayerAction action = strategy->soft_totals[dealer_idx][SOFT_IDX(player_hand_value)];
        switch (action) {
            case SOFT_HIT:
                return HIT;
            case SOFT_STAND:
                return STAND;
            case SOFT_DOUBLE_OR_HIT:
                if (can_double && rules->double_any_two_cards) {
                    return DOUBLE;
                }
                return HIT;
            case SOFT_DOUBLE_OR_STAND:
                if (can_double && rules->double_any_two_cards) {
                    return DOUBLE;
                }
                return STAND;
        }
    }

    // 3. Check if should surrender and look up surrender table
    if (rules->late_surrender_allowed && player_hand_value >= 14 && player_hand_value <= 16) {
        int surrender_idx = player_hand_value - 14;  // 14->0, 15->1, 16->2
        if (strategy->surrender[dealer_idx][surrender_idx]) {
            return SURRENDER;
        }
    }

    // 4. Otherwise look up hard_totals table
    // Hard totals table only covers 8-16; handle edge cases
    if (player_hand_value >= 17) {
        return STAND;  // Always stand on 17+
    }
    if (player_hand_value < 8) {
        return HIT;    // Always hit on < 8
    }

    PlayerAction action = strategy->hard_totals[dealer_idx][HARD_IDX(player_hand_value)];
    if (!can_double && action == DOUBLE) {
        return HIT;
    }

    return action;
}
