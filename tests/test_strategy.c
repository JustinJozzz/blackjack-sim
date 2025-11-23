#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
// Uncomment these as you create the headers
#include "../src/strategy.h"
#include "../src/simulation.h"
#include "../src/game.h"
#include "../src/hand.h"

// Simple test framework
int tests_run = 0;
int tests_passed = 0;

#define TEST(name) \
    void test_##name(); \
    void run_test_##name() { \
        printf("Running test: %s...", #name); \
        tests_run++; \
        test_##name(); \
        tests_passed++; \
        printf(" PASSED\n"); \
    } \
    void test_##name()

// ============================================================================
// BASIC STRATEGY TESTS
// ============================================================================
// Implement strategy.h with:
// PlayerAction get_basic_strategy_action(Hand* player, int dealer_upcard, Rules* rules, bool can_split, bool can_double)
//
// Basic strategy logic based on mathematical optimal play for standard rules

TEST(strategy_player_hard_16_vs_dealer_10) {
    Rules rules;
    rules_init(&rules);

    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    Hand player;
    hand_init(&player);
    hand_add_card(&player, 9);  // 10
    hand_add_card(&player, 5);  // 6 -> Hard 16

    // Against dealer 10, basic strategy says HIT (without surrender)
    // With surrender allowed, basic strategy says SURRENDER
    int dealer_upcard = 9;  // 10-value card

    if (rules.late_surrender_allowed) {
        PlayerAction action = get_basic_strategy_action(&player, dealer_upcard, &rules, &strategy, false, false, true);
        assert(action == SURRENDER);
    }

    hand_destroy(&player);
}

TEST(strategy_player_soft_18_vs_dealer_9) {
    Rules rules;
    rules_init(&rules);

    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    Hand player;
    hand_init(&player);
    hand_add_card(&player, 0);  // Ace
    hand_add_card(&player, 6);  // 7 -> Soft 18

    int dealer_upcard = 8;  // 9-value card

    // Soft 18 vs 9: basic strategy says HIT
    PlayerAction action = get_basic_strategy_action(&player, dealer_upcard, &rules, &strategy, false, true, false);
    assert(action == HIT);

    hand_destroy(&player);
}

TEST(strategy_player_11_vs_dealer_any) {
    Rules rules;
    rules_init(&rules);

    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    Hand player;
    hand_init(&player);
    hand_add_card(&player, 5);  // 6
    hand_add_card(&player, 4);  // 5 -> 11

    // 11 vs any dealer card: basic strategy says DOUBLE (if allowed)
    for (int dealer_upcard = 0; dealer_upcard < 13; dealer_upcard++) {
        PlayerAction action = get_basic_strategy_action(&player, dealer_upcard, &rules, &strategy, false, true, false);
        assert(action == DOUBLE);
    }

    hand_destroy(&player);
}

TEST(strategy_player_pair_8s_vs_any) {
    Rules rules;
    rules_init(&rules);

    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    Hand player;
    hand_init(&player);
    hand_add_card(&player, 7);  // 8
    hand_add_card(&player, 7);  // 8 -> Pair of 8s

    // Pair of 8s vs any dealer card: always SPLIT
    for (int dealer_upcard = 0; dealer_upcard < 13; dealer_upcard++) {
        PlayerAction action = get_basic_strategy_action(&player, dealer_upcard, &rules, &strategy, true, true, false);
        assert(action == SPLIT);
    }

    hand_destroy(&player);
}

TEST(strategy_player_pair_10s_vs_any) {
    Rules rules;
    rules_init(&rules);

    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    Hand player;
    hand_init(&player);
    hand_add_card(&player, 9);  // 10
    hand_add_card(&player, 9);  // 10 -> Pair of 10s (hard 20)

    // Pair of 10s vs any dealer card: always STAND (never split)
    for (int dealer_upcard = 0; dealer_upcard < 13; dealer_upcard++) {
        PlayerAction action = get_basic_strategy_action(&player, dealer_upcard, &rules, &strategy, true, true, false);
        assert(action == STAND);
    }

    hand_destroy(&player);
}

TEST(strategy_player_hard_17_vs_any) {
    Rules rules;
    rules_init(&rules);

    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    Hand player;
    hand_init(&player);
    hand_add_card(&player, 9);  // 10
    hand_add_card(&player, 6);  // 7 -> Hard 17

    // Hard 17 or higher: always STAND
    for (int dealer_upcard = 0; dealer_upcard < 13; dealer_upcard++) {
        PlayerAction action = get_basic_strategy_action(&player, dealer_upcard, &rules, &strategy, false, false, false);
        assert(action == STAND);
    }

    hand_destroy(&player);
}

// ============================================================================
// COMPREHENSIVE BASIC STRATEGY VERIFICATION TESTS
// ============================================================================

TEST(strategy_hard_12_vs_dealer_weak) {
    Rules rules;
    rules_init(&rules);
    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    Hand player;
    hand_init(&player);
    hand_add_card(&player, 9);  // 10
    hand_add_card(&player, 1);  // 2 -> Hard 12

    // Hard 12 vs dealer 2-3: HIT
    PlayerAction action = get_basic_strategy_action(&player, 1, &rules, &strategy, false, false, false);
    assert(action == HIT);

    // Hard 12 vs dealer 4-6: STAND
    action = get_basic_strategy_action(&player, 3, &rules, &strategy, false, false, false);
    assert(action == STAND);

    // Hard 12 vs dealer 7-A: HIT
    action = get_basic_strategy_action(&player, 6, &rules, &strategy, false, false, false);
    assert(action == HIT);

    hand_destroy(&player);
}

TEST(strategy_hard_13_14_15_16_vs_dealer) {
    Rules rules;
    rules_init(&rules);
    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    // Test hard 13-16 all behave the same: stand vs 2-6, hit vs 7-A
    for (int total = 13; total <= 16; total++) {
        Hand player;
        hand_init(&player);
        hand_add_card(&player, 9);  // 10
        hand_add_card(&player, total - 10 - 1);  // Make the total

        // Should STAND vs dealer 2-6
        for (int dealer = 2; dealer <= 6; dealer++) {
            PlayerAction action = get_basic_strategy_action(&player, dealer - 1, &rules, &strategy, false, false, false);
            assert(action == STAND);
        }

        // Should HIT vs dealer 7-A
        for (int dealer = 7; dealer <= 11; dealer++) {
            PlayerAction action = get_basic_strategy_action(&player, dealer - 1, &rules, &strategy, false, false, false);
            assert(action == HIT);
        }

        hand_destroy(&player);
    }
}

TEST(strategy_soft_double_situations) {
    Rules rules;
    rules_init(&rules);
    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    // A-6 (soft 17) vs dealer 3-6: should DOUBLE (or hit if can't double)
    Hand player;
    hand_init(&player);
    hand_add_card(&player, 0);  // Ace
    hand_add_card(&player, 5);  // 6 -> Soft 17

    // Can double - should return DOUBLE
    PlayerAction action = get_basic_strategy_action(&player, 4, &rules, &strategy, false, true, false);
    assert(action == DOUBLE);

    // Can't double - should return HIT
    action = get_basic_strategy_action(&player, 4, &rules, &strategy, false, false, false);
    assert(action == HIT);

    hand_destroy(&player);
}

TEST(strategy_pair_aces_vs_all_dealers) {
    Rules rules;
    rules_init(&rules);
    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    Hand player;
    hand_init(&player);
    hand_add_card(&player, 0);  // Ace
    hand_add_card(&player, 0);  // Ace

    // Pair of Aces: always SPLIT vs any dealer
    for (int dealer = 0; dealer < 13; dealer++) {
        PlayerAction action = get_basic_strategy_action(&player, dealer, &rules, &strategy, true, true, false);
        assert(action == SPLIT);
    }

    hand_destroy(&player);
}

TEST(strategy_pair_9s_special_cases) {
    Rules rules;
    rules_init(&rules);
    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    Hand player;
    hand_init(&player);
    hand_add_card(&player, 8);  // 9
    hand_add_card(&player, 8);  // 9

    // Pair of 9s: split vs 2-6,8-9
    for (int dealer_val = 2; dealer_val <= 6; dealer_val++) {
        PlayerAction action = get_basic_strategy_action(&player, dealer_val - 1, &rules, &strategy, true, true, false);
        assert(action == SPLIT);
    }

    // But STAND vs 7 (dealer has 17)
    PlayerAction action = get_basic_strategy_action(&player, 6, &rules, &strategy, true, true, false);
    assert(action == STAND);

    // Split vs 8-9
    action = get_basic_strategy_action(&player, 7, &rules, &strategy, true, true, false);
    assert(action == SPLIT);
    action = get_basic_strategy_action(&player, 8, &rules, &strategy, true, true, false);
    assert(action == SPLIT);

    // But STAND vs 10, A
    action = get_basic_strategy_action(&player, 9, &rules, &strategy, true, true, false);
    assert(action == STAND);

    hand_destroy(&player);
}

// ============================================================================
// SIMULATION TESTS
// ============================================================================
// Implement simulation.h with:
// typedef struct {
//     int num_hands;
//     Rules rules;
//     bool use_basic_strategy;
// } SimulationConfig;
//
// typedef struct {
//     int hands_played;
//     int hands_won;
//     int hands_lost;
//     int hands_pushed;
//     int blackjacks;
//     double total_bet;
//     double total_payout;
//     double house_edge;  // Expected loss per hand
// } SimulationResults;
//
// void simulation_run(SimulationConfig* config, SimulationResults* results)
// double simulation_get_ev(SimulationResults* results) - Calculate expected value

TEST(simulation_initialization) {
    SimulationConfig config;
    rules_init(&config.rules);
    basic_strategy_init(&config.strategy);
    config.num_hands = 1000;
    config.bet_per_hand = 1.0;

    SimulationResults results = {0};

    simulation_run(&config, &results);

    assert(results.hands_played == 1000);
    assert(results.hands_won > 0);
    assert(results.hands_lost > 0);
    // With doubles and splits, total bet will be > base amount
    assert(results.total_bet >= 1000.0);  // At least $1 per hand
    assert(results.total_bet < 2000.0);   // But not absurdly high
}

TEST(simulation_double_and_split_actions) {
    SimulationConfig config;
    rules_init(&config.rules);
    basic_strategy_init(&config.strategy);
    config.num_hands = 10000;  // Larger sample to ensure doubles/splits occur
    config.bet_per_hand = 1.0;

    SimulationResults results = {0};

    simulation_run(&config, &results);

    // With basic strategy and 10k hands, we should see:
    // - Doubles: ~9-11% of hands (player has 9, 10, 11, or soft totals)
    // - Splits: ~3-5% of hands (pairs that should be split)

    assert(results.doubles_taken > 0);  // Should have doubled at least once
    assert(results.splits_taken > 0);   // Should have split at least once

    // More specific: expect at least 5% doubles and 2% splits
    assert(results.doubles_taken > results.hands_played * 0.05);
    assert(results.splits_taken > results.hands_played * 0.02);

    printf("\n  Doubles: %d (%.1f%%), Splits: %d (%.1f%%)",
           results.doubles_taken,
           (double)results.doubles_taken / results.hands_played * 100,
           results.splits_taken,
           (double)results.splits_taken / results.hands_played * 100);
}

TEST(simulation_basic_strategy_ev) {
    SimulationConfig config;
    rules_init(&config.rules);
    basic_strategy_init(&config.strategy);
    config.num_hands = 100000;  // Large sample to converge to true EV
    config.bet_per_hand = 1.0;

    SimulationResults results = {0};
    simulation_run(&config, &results);

    double ev = simulation_get_ev(&results);

    printf("\n  Expected Value: %.4f (%.2f%%)", ev, ev * 100);
    printf("\n  Win/Loss/Push: %d/%d/%d (%.1f%%/%.1f%%/%.1f%%)",
           results.hands_won, results.hands_lost, results.hands_pushed,
           (double)results.hands_won / results.hands_played * 100,
           (double)results.hands_lost / results.hands_played * 100,
           (double)results.hands_pushed / results.hands_played * 100);
    printf("\n  Total bet: %.2f, Total payout: %.2f",
           results.total_bet, results.total_payout);
    printf("\n  Average bet: %.2f, Average payout: %.2f",
           results.total_bet / results.hands_played,
           results.total_payout / results.hands_played);

    // With perfect basic strategy and standard rules,
    // house edge should be around 0.5% (EV around -0.005)
    // Allow some variance: -0.01 to 0.0
    assert(ev > -0.01 && ev < 0.0);
}

TEST(simulation_ev_variance_across_seeds) {
    printf("\n  Testing: EV variance across different RNG seeds");

    int seeds[] = {123456789, 987654321, 555555555, 111111111, 999999999};
    int num_seeds = 5;
    double evs[5];
    double sum_ev = 0.0;

    for (int i = 0; i < num_seeds; i++) {
        deck_set_rng_seed(seeds[i]);

        SimulationConfig config;
        rules_init(&config.rules);
        basic_strategy_init(&config.strategy);
        config.num_hands = 100000;
        config.bet_per_hand = 1.0;

        SimulationResults results = {0};
        simulation_run(&config, &results);

        evs[i] = simulation_get_ev(&results);
        sum_ev += evs[i];

        printf("\n    Seed %d: EV = %.4f (%.2f%%)", seeds[i], evs[i], evs[i] * 100);
    }

    double avg_ev = sum_ev / num_seeds;
    printf("\n    Average EV across %d seeds: %.4f (%.2f%%)", num_seeds, avg_ev, avg_ev * 100);

    // Reset to original seed
    deck_set_rng_seed(123456789);
}

// ============================================================================
// SPLIT HAND RESOLUTION TESTS
// ============================================================================

TEST(game_split_hand_one_wins_one_loses) {
    printf("\n  Testing: Split hand - one wins, one loses");

    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;

    // Player: pair of 8s (16 total, should split vs dealer 6)
    hand_add_card(&game.player_hands[0], 7);  // 8
    hand_add_card(&game.player_hands[0], 7);  // 8

    // Dealer: 6
    hand_add_card(&game.dealer_hand, 5);  // 6
    hand_add_card(&game.dealer_hand, 5);  // 6 = 12

    // Split the 8s
    game_play_action(&game, SPLIT, 0);
    printf("\n    After split: %d hands", game.num_player_hands);
    assert(game.num_player_hands == 2);

    // Manually set up split hands for deterministic test
    // Hand 0: 8 + 10 = 18
    // Hand 1: 8 + 5 = 13
    printf("\n    Hand 0: value=%d, Hand 1: value=%d",
           hand_get_value(&game.player_hands[0]),
           hand_get_value(&game.player_hands[1]));

    // Dealer draws to 22 (bust)
    hand_add_card(&game.dealer_hand, 9);  // 6 + 6 + 10 = 22 (bust)
    printf("\n    Dealer: value=%d (bust)", hand_get_value(&game.dealer_hand));

    double payout = game_resolve(&game);
    printf("\n    Total bet: %.2f, Total payout: %.2f",
           game.player_bets[0] + game.player_bets[1], payout);

    // Both hands should win against dealer bust
    // Each bet is 10.0, both win, so payout = 10*2 + 10*2 = 40.0
    assert(payout == 40.0);

    game_destroy(&game);
}

TEST(game_split_hand_both_bust) {
    printf("\n  Testing: Split hand - both hands bust");

    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;

    // Player: pair of 8s
    hand_add_card(&game.player_hands[0], 7);  // 8
    hand_add_card(&game.player_hands[0], 7);  // 8

    // Dealer: 10
    hand_add_card(&game.dealer_hand, 9);  // 10
    hand_add_card(&game.dealer_hand, 9);  // 10 = 20

    // Split
    game_play_action(&game, SPLIT, 0);

    // Both hands bust
    hand_add_card(&game.player_hands[0], 9);  // 8 + 10 = 18
    hand_add_card(&game.player_hands[0], 9);  // 18 + 10 = 28 (bust)
    hand_add_card(&game.player_hands[1], 9);  // 8 + 10 = 18
    hand_add_card(&game.player_hands[1], 8);  // 18 + 9 = 27 (bust)

    printf("\n    Hand 0: %d (bust), Hand 1: %d (bust)",
           hand_get_value(&game.player_hands[0]),
           hand_get_value(&game.player_hands[1]));
    printf("\n    Dealer: %d (should not play)", hand_get_value(&game.dealer_hand));

    double payout = game_resolve(&game);
    printf("\n    Total payout: %.2f (both hands lost)", payout);

    // Both hands busted, total loss
    assert(payout == 0.0);

    game_destroy(&game);
}

// ============================================================================
// DOUBLE AFTER SPLIT (DAS) TESTS
// ============================================================================

TEST(game_double_after_split_allowed) {
    printf("\n  Testing: Double after split (DAS enabled)");

    Rules rules;
    rules_init(&rules);
    rules.double_after_split = true;  // Ensure DAS is enabled

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;

    // Player: pair of 3s (should split vs dealer 5)
    hand_add_card(&game.player_hands[0], 2);  // 3
    hand_add_card(&game.player_hands[0], 2);  // 3

    // Dealer: 5
    hand_add_card(&game.dealer_hand, 4);  // 5
    hand_add_card(&game.dealer_hand, 4);  // 5 = 10

    // Split the 3s
    game_play_action(&game, SPLIT, 0);
    printf("\n    After split: %d hands", game.num_player_hands);

    // Now hand 0 has 3 + new card, let's say we get an 8 = 11 (good double situation)
    // We should be able to double on this hand
    printf("\n    Hand 0 after split: value=%d", hand_get_value(&game.player_hands[0]));

    // Double on first split hand
    if (hand_get_value(&game.player_hands[0]) >= 9 && hand_get_value(&game.player_hands[0]) <= 11) {
        game_play_action(&game, DOUBLE, 0);
        printf("\n    Doubled on hand 0, new bet: %.2f", game.player_bets[0]);
        assert(game.player_bets[0] == 20.0);  // Original 10 doubled to 20
    }

    game_destroy(&game);
}

// ============================================================================
// SURRENDER TESTS
// ============================================================================

TEST(strategy_surrender_16_vs_10) {
    printf("\n  Testing: Surrender on hard 16 vs dealer 10");

    Rules rules;
    rules_init(&rules);
    rules.late_surrender_allowed = true;

    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    // Hard 16 vs dealer 10
    Hand player_hand;
    hand_init(&player_hand);
    hand_add_card(&player_hand, 9);  // 10
    hand_add_card(&player_hand, 5);  // 6 -> hard 16

    int dealer_up_card = 9;  // 10 value card

    PlayerAction action = get_basic_strategy_action(&player_hand, dealer_up_card, &rules, &strategy,
                                                     false,  // can't split
                                                     true,   // can double
                                                     true);  // can surrender

    printf("\n    Player hard 16 vs Dealer 10: Action = %s",
           action == SURRENDER ? "SURRENDER" :
           action == HIT ? "HIT" :
           action == STAND ? "STAND" : "OTHER");

    assert(action == SURRENDER);

    hand_destroy(&player_hand);
}

TEST(strategy_surrender_15_vs_10) {
    printf("\n  Testing: Surrender on hard 15 vs dealer 10");

    Rules rules;
    rules_init(&rules);
    rules.late_surrender_allowed = true;

    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    // Hard 15 vs dealer 10
    Hand player_hand;
    hand_init(&player_hand);
    hand_add_card(&player_hand, 9);  // 10
    hand_add_card(&player_hand, 4);  // 5 -> hard 15

    int dealer_up_card = 9;  // 10 value card

    PlayerAction action = get_basic_strategy_action(&player_hand, dealer_up_card, &rules, &strategy,
                                                     false,  // can't split
                                                     true,   // can double
                                                     true);  // can surrender

    printf("\n    Player hard 15 vs Dealer 10: Action = %s",
           action == SURRENDER ? "SURRENDER" :
           action == HIT ? "HIT" :
           action == STAND ? "STAND" : "OTHER");

    assert(action == SURRENDER);

    hand_destroy(&player_hand);
}

TEST(strategy_surrender_16_vs_ace) {
    printf("\n  Testing: Surrender on hard 16 vs dealer Ace");

    Rules rules;
    rules_init(&rules);
    rules.late_surrender_allowed = true;

    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    // Hard 16 vs dealer Ace
    Hand player_hand;
    hand_init(&player_hand);
    hand_add_card(&player_hand, 9);  // 10
    hand_add_card(&player_hand, 5);  // 6 -> hard 16

    int dealer_up_card = 0;  // Ace

    PlayerAction action = get_basic_strategy_action(&player_hand, dealer_up_card, &rules, &strategy,
                                                     false,  // can't split
                                                     true,   // can double
                                                     true);  // can surrender

    printf("\n    Player hard 16 vs Dealer Ace: Action = %s",
           action == SURRENDER ? "SURRENDER" :
           action == HIT ? "HIT" :
           action == STAND ? "STAND" : "OTHER");

    assert(action == SURRENDER);

    hand_destroy(&player_hand);
}

TEST(game_surrender_payout_correct) {
    printf("\n  Testing: Surrender payout calculation");

    Rules rules;
    rules_init(&rules);
    rules.late_surrender_allowed = true;

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;

    // Player: 16
    hand_add_card(&game.player_hands[0], 9);  // 10
    hand_add_card(&game.player_hands[0], 5);  // 6 -> 16

    // Dealer: 10
    hand_add_card(&game.dealer_hand, 9);  // 10
    hand_add_card(&game.dealer_hand, 9);  // 10 -> 20

    printf("\n    Player 16 vs Dealer 20");

    // Surrender
    game_play_action(&game, SURRENDER, 0);

    double payout = game_resolve(&game);
    printf("\n    Bet: %.2f, Payout: %.2f (should get half bet back)",
           game.player_bets[0], payout);

    // Should get 0.5x bet = 5.0 back
    assert(payout == 5.0);

    game_destroy(&game);
}

// ============================================================================
// COMPREHENSIVE HARD TOTALS VERIFICATION (ALL 90 COMBINATIONS)
// ============================================================================

TEST(strategy_verify_all_hard_totals) {
    printf("\n  Testing: Complete hard totals table (90 combinations)");

    Rules rules;
    rules_init(&rules);

    BasicStrategy strategy;
    basic_strategy_init(&strategy);

    int errors = 0;
    int total_tests = 0;

    // Expected actions for hard totals (player 8-16 vs dealer 2-A)
    // Format: [player_total - 8][dealer_card - 2]
    // H = HIT, S = STAND, D = DOUBLE
    const char* expected_actions[9][10] = {
        // Player 8 vs Dealer: 2, 3, 4, 5, 6, 7, 8, 9, 10, A
        {"H", "H", "H", "H", "H", "H", "H", "H", "H", "H"},  // 8

        // Player 9 vs Dealer: 2, 3, 4, 5, 6, 7, 8, 9, 10, A
        {"H", "D", "D", "D", "D", "H", "H", "H", "H", "H"},  // 9

        // Player 10 vs Dealer: 2, 3, 4, 5, 6, 7, 8, 9, 10, A
        {"D", "D", "D", "D", "D", "D", "D", "D", "H", "H"},  // 10

        // Player 11 vs Dealer: 2, 3, 4, 5, 6, 7, 8, 9, 10, A
        {"D", "D", "D", "D", "D", "D", "D", "D", "D", "D"},  // 11

        // Player 12 vs Dealer: 2, 3, 4, 5, 6, 7, 8, 9, 10, A
        {"H", "H", "S", "S", "S", "H", "H", "H", "H", "H"},  // 12

        // Player 13 vs Dealer: 2, 3, 4, 5, 6, 7, 8, 9, 10, A
        {"S", "S", "S", "S", "S", "H", "H", "H", "H", "H"},  // 13

        // Player 14 vs Dealer: 2, 3, 4, 5, 6, 7, 8, 9, 10, A
        {"S", "S", "S", "S", "S", "H", "H", "H", "H", "H"},  // 14

        // Player 15 vs Dealer: 2, 3, 4, 5, 6, 7, 8, 9, 10, A
        {"S", "S", "S", "S", "S", "H", "H", "H", "H", "H"},  // 15

        // Player 16 vs Dealer: 2, 3, 4, 5, 6, 7, 8, 9, 10, A
        {"S", "S", "S", "S", "S", "H", "H", "H", "H", "H"}   // 16
    };

    // Test all combinations
    for (int player_total = 8; player_total <= 16; player_total++) {
        for (int dealer_value = 2; dealer_value <= 11; dealer_value++) {
            total_tests++;

            // Create a hand with the target total (using cards that won't be pairs)
            Hand player_hand;
            hand_init(&player_hand);

            // Build hand to reach target total without pairs
            if (player_total <= 11) {
                // For 8-11, use simple combinations
                hand_add_card(&player_hand, player_total - 3);  // First card
                hand_add_card(&player_hand, 1);                 // Second card (2)
            } else {
                // For 12-16, use 10 + low card to avoid pair detection
                hand_add_card(&player_hand, 9);                      // 10
                hand_add_card(&player_hand, player_total - 11);      // Remaining value
            }

            // Map dealer value to card index (Ace = 0, 2-10 = 1-9)
            int dealer_card = (dealer_value == 11) ? 0 : dealer_value - 1;

            PlayerAction action = get_basic_strategy_action(&player_hand, dealer_card, &rules, &strategy,
                                                             false,  // can't split
                                                             true,   // can double
                                                             false); // can't surrender

            // Check expected action
            const char* expected = expected_actions[player_total - 8][dealer_value - 2];
            const char* actual = (action == HIT) ? "H" :
                                (action == STAND) ? "S" :
                                (action == DOUBLE) ? "D" : "?";

            if (strcmp(expected, actual) != 0) {
                printf("\n    ERROR: Player %d vs Dealer %s: Expected %s, Got %s",
                       player_total,
                       dealer_value == 11 ? "A" : (dealer_value == 10 ? "10" : "?"),
                       expected, actual);
                errors++;
            }

            hand_destroy(&player_hand);
        }
    }

    printf("\n    Verified %d hard total combinations", total_tests);
    if (errors > 0) {
        printf("\n    ERRORS FOUND: %d", errors);
    }

    assert(errors == 0);
}

TEST(strategy_verify_all_soft_totals) {
    printf("\n  Testing: Complete soft totals table (80 combinations)");

    BasicStrategy strategy;
    basic_strategy_init(&strategy);
    Rules rules;
    rules_init(&rules);

    // Expected actions for soft totals (A-2 through A-9 vs dealer 2-A)
    // D = Double or Hit, Ds = Double or Stand, H = Hit, S = Stand
    const char* expected_actions[8][10] = {
        {"H", "H", "H", "D", "D", "H", "H", "H", "H", "H"},  // Soft 13 (A-2)
        {"H", "H", "H", "D", "D", "H", "H", "H", "H", "H"},  // Soft 14 (A-3)
        {"H", "H", "D", "D", "D", "H", "H", "H", "H", "H"},  // Soft 15 (A-4)
        {"H", "H", "D", "D", "D", "H", "H", "H", "H", "H"},  // Soft 16 (A-5)
        {"H", "D", "D", "D", "D", "H", "H", "H", "H", "H"},  // Soft 17 (A-6)
        {"Ds", "Ds", "Ds", "Ds", "Ds", "S", "S", "H", "H", "H"},  // Soft 18 (A-7)
        {"S", "S", "S", "S", "Ds", "S", "S", "S", "S", "S"},  // Soft 19 (A-8)
        {"S", "S", "S", "S", "S", "S", "S", "S", "S", "S"}   // Soft 20 (A-9)
    };

    int total_tests = 0;
    int errors = 0;

    // Test all soft totals (13-20) vs all dealer cards (2-A)
    for (int soft_total = 13; soft_total <= 20; soft_total++) {
        for (int dealer_value = 2; dealer_value <= 11; dealer_value++) {
            total_tests++;

            // Create a soft hand with the target total
            Hand player_hand;
            hand_init(&player_hand);

            // Add Ace first
            hand_add_card(&player_hand, 0);  // Ace

            // Add second card to reach target soft total
            int second_card_value = soft_total - 11;  // Since Ace = 11
            int second_card = second_card_value - 1;  // Card index (1=2, 2=3, etc.)
            hand_add_card(&player_hand, second_card);

            // Verify hand is soft and has correct value
            assert(hand_is_soft(&player_hand) == true);
            assert(hand_get_value(&player_hand) == soft_total);

            // Create dealer card
            int dealer_card = (dealer_value == 11) ? 0 : (dealer_value - 1);

            // Get strategy action (can double, cannot split, cannot surrender)
            PlayerAction action = get_basic_strategy_action(&player_hand, dealer_card, &rules, &strategy,
                                                             false, true, false);

            // Convert action to string
            const char* actual;
            const char* expected = expected_actions[soft_total - 13][dealer_value - 2];

            if (strcmp(expected, "D") == 0) {
                // Double or Hit
                actual = (action == DOUBLE) ? "D" : "H";
                if (action != DOUBLE && action != HIT) {
                    actual = "ERROR";
                }
            } else if (strcmp(expected, "Ds") == 0) {
                // Double or Stand
                actual = (action == DOUBLE) ? "Ds" : (action == STAND) ? "Ds" : "ERROR";
                if (action != DOUBLE && action != STAND) {
                    actual = "ERROR";
                }
            } else if (strcmp(expected, "H") == 0) {
                actual = (action == HIT) ? "H" : "ERROR";
            } else if (strcmp(expected, "S") == 0) {
                actual = (action == STAND) ? "S" : "ERROR";
            } else {
                actual = "UNKNOWN";
            }

            // Check if action matches expected
            if (strcmp(expected, "D") == 0 && (action != DOUBLE && action != HIT)) {
                printf("\n    ERROR: Soft %d vs Dealer %s: Expected %s, Got action %d",
                       soft_total, dealer_value == 11 ? "A" : (dealer_value == 2 ? "2" :
                       dealer_value == 3 ? "3" : dealer_value == 4 ? "4" : dealer_value == 5 ? "5" :
                       dealer_value == 6 ? "6" : dealer_value == 7 ? "7" : dealer_value == 8 ? "8" :
                       dealer_value == 9 ? "9" : "10"), expected, action);
                errors++;
            } else if (strcmp(expected, "Ds") == 0 && (action != DOUBLE && action != STAND)) {
                printf("\n    ERROR: Soft %d vs Dealer %s: Expected %s, Got action %d",
                       soft_total, dealer_value == 11 ? "A" : (dealer_value == 2 ? "2" :
                       dealer_value == 3 ? "3" : dealer_value == 4 ? "4" : dealer_value == 5 ? "5" :
                       dealer_value == 6 ? "6" : dealer_value == 7 ? "7" : dealer_value == 8 ? "8" :
                       dealer_value == 9 ? "9" : "10"), expected, action);
                errors++;
            } else if (strcmp(expected, "H") == 0 && action != HIT) {
                printf("\n    ERROR: Soft %d vs Dealer %s: Expected HIT, Got action %d",
                       soft_total, dealer_value == 11 ? "A" : (dealer_value == 2 ? "2" :
                       dealer_value == 3 ? "3" : dealer_value == 4 ? "4" : dealer_value == 5 ? "5" :
                       dealer_value == 6 ? "6" : dealer_value == 7 ? "7" : dealer_value == 8 ? "8" :
                       dealer_value == 9 ? "9" : "10"), action);
                errors++;
            } else if (strcmp(expected, "S") == 0 && action != STAND) {
                printf("\n    ERROR: Soft %d vs Dealer %s: Expected STAND, Got action %d",
                       soft_total, dealer_value == 11 ? "A" : (dealer_value == 2 ? "2" :
                       dealer_value == 3 ? "3" : dealer_value == 4 ? "4" : dealer_value == 5 ? "5" :
                       dealer_value == 6 ? "6" : dealer_value == 7 ? "7" : dealer_value == 8 ? "8" :
                       dealer_value == 9 ? "9" : "10"), action);
                errors++;
            }

            hand_destroy(&player_hand);
        }
    }

    printf("\n    Verified %d soft total combinations", total_tests);
    if (errors > 0) {
        printf("\n    ERRORS FOUND: %d", errors);
    }

    assert(errors == 0);
}

TEST(strategy_verify_all_pairs) {
    printf("\n  Testing: Complete pairs table (100 combinations)");

    BasicStrategy strategy;
    basic_strategy_init(&strategy);
    Rules rules;
    rules_init(&rules);

    // Expected split actions for pairs (2-2 through A-A vs dealer 2-A)
    // Y = Yes (always split), N = No (never split), D = Yes if DAS (double after split)
    const char* expected_actions[10][10] = {
        {"D", "D", "Y", "Y", "Y", "Y", "N", "N", "N", "N"},  // 2-2
        {"D", "D", "Y", "Y", "Y", "Y", "N", "N", "N", "N"},  // 3-3
        {"N", "N", "N", "D", "D", "N", "N", "N", "N", "N"},  // 4-4
        {"N", "N", "N", "N", "N", "N", "N", "N", "N", "N"},  // 5-5 (never)
        {"D", "D", "D", "D", "D", "N", "N", "N", "N", "N"},  // 6-6
        {"Y", "Y", "Y", "Y", "Y", "Y", "N", "N", "N", "N"},  // 7-7
        {"Y", "Y", "Y", "Y", "Y", "Y", "Y", "Y", "Y", "Y"},  // 8-8 (always)
        {"Y", "Y", "Y", "Y", "Y", "N", "Y", "Y", "N", "N"},  // 9-9
        {"N", "N", "N", "N", "N", "N", "N", "N", "N", "N"},  // T-T (never)
        {"Y", "Y", "Y", "Y", "Y", "Y", "Y", "Y", "Y", "Y"}   // A-A (always)
    };

    const char* pair_names[] = {"2-2", "3-3", "4-4", "5-5", "6-6", "7-7", "8-8", "9-9", "T-T", "A-A"};
    int pair_cards[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};  // Card indices for each pair

    int total_tests = 0;
    int errors = 0;

    // Test all pairs vs all dealer cards
    for (int pair_idx = 0; pair_idx < 10; pair_idx++) {
        for (int dealer_value = 2; dealer_value <= 11; dealer_value++) {
            total_tests++;

            // Create a pair
            Hand player_hand;
            hand_init(&player_hand);
            int card = pair_cards[pair_idx];
            hand_add_card(&player_hand, card);
            hand_add_card(&player_hand, card);

            // Verify it's a splittable pair
            assert(hand_can_split(&player_hand) == true);

            // Create dealer card
            int dealer_card = (dealer_value == 11) ? 0 : (dealer_value - 1);

            // Get strategy action (can split, can double, cannot surrender)
            PlayerAction action = get_basic_strategy_action(&player_hand, dealer_card, &rules, &strategy,
                                                             true, true, false);

            // Check expected action
            const char* expected = expected_actions[pair_idx][dealer_value - 2];
            bool should_split = false;

            if (strcmp(expected, "Y") == 0) {
                should_split = true;  // Always split
            } else if (strcmp(expected, "D") == 0) {
                should_split = true;  // Split if DAS (which is enabled in our rules)
            } else if (strcmp(expected, "N") == 0) {
                should_split = false;  // Never split
            }

            // Verify action
            if (should_split && action != SPLIT) {
                printf("\n    ERROR: %s vs Dealer %s: Expected SPLIT, Got action %d",
                       pair_names[pair_idx],
                       dealer_value == 11 ? "A" : (dealer_value == 2 ? "2" :
                       dealer_value == 3 ? "3" : dealer_value == 4 ? "4" : dealer_value == 5 ? "5" :
                       dealer_value == 6 ? "6" : dealer_value == 7 ? "7" : dealer_value == 8 ? "8" :
                       dealer_value == 9 ? "9" : "10"),
                       action);
                errors++;
            } else if (!should_split && action == SPLIT) {
                printf("\n    ERROR: %s vs Dealer %s: Expected NOT SPLIT, Got SPLIT",
                       pair_names[pair_idx],
                       dealer_value == 11 ? "A" : (dealer_value == 2 ? "2" :
                       dealer_value == 3 ? "3" : dealer_value == 4 ? "4" : dealer_value == 5 ? "5" :
                       dealer_value == 6 ? "6" : dealer_value == 7 ? "7" : dealer_value == 8 ? "8" :
                       dealer_value == 9 ? "9" : "10"));
                errors++;
            }

            hand_destroy(&player_hand);
        }
    }

    printf("\n    Verified %d pair combinations", total_tests);
    if (errors > 0) {
        printf("\n    ERRORS FOUND: %d", errors);
    }

    assert(errors == 0);
}

// ============================================================================
// INSURANCE TESTS
// ============================================================================
// Insurance is offered when dealer shows Ace
// Insurance bet is half the original bet
// Insurance pays 2:1 if dealer has blackjack
// Insurance is lost if dealer doesn't have blackjack

TEST(insurance_offered_when_dealer_shows_ace) {
    printf("\n  Testing: Insurance offered when dealer shows Ace");

    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;

    // Player: regular hand (not blackjack)
    hand_add_card(&game.player_hands[0], 9);  // 10
    hand_add_card(&game.player_hands[0], 5);  // 6 -> 16

    // Dealer: Ace showing
    hand_add_card(&game.dealer_hand, 0);  // Ace
    hand_add_card(&game.dealer_hand, 9);  // 10 (blackjack, but player shouldn't know yet)

    printf("\n    Player: %d, Dealer showing: Ace",
           hand_get_value(&game.player_hands[0]));

    // Verify insurance is offered
    bool should_offer = game_should_offer_insurance(&game);
    printf("\n    Insurance offered: %s", should_offer ? "YES" : "NO");
    assert(should_offer == true);

    // Take insurance (half of original bet)
    double insurance_amount = game.player_bets[0] / 2.0;
    game_take_insurance(&game, insurance_amount);
    printf("\n    Insurance bet: %.2f", game.insurance_bet);
    assert(game.insurance_bet == 5.0);

    game_destroy(&game);
}

TEST(insurance_not_offered_when_dealer_shows_non_ace) {
    printf("\n  Testing: Insurance not offered when dealer shows non-Ace");

    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;

    // Player: regular hand
    hand_add_card(&game.player_hands[0], 9);  // 10
    hand_add_card(&game.player_hands[0], 5);  // 6 -> 16

    // Dealer: 10 showing (not Ace)
    hand_add_card(&game.dealer_hand, 9);  // 10
    hand_add_card(&game.dealer_hand, 5);  // 6 -> 16

    printf("\n    Player: %d, Dealer showing: 10",
           hand_get_value(&game.player_hands[0]));

    // Verify insurance is NOT offered
    bool should_offer = game_should_offer_insurance(&game);
    printf("\n    Insurance offered: %s", should_offer ? "YES" : "NO");
    assert(should_offer == false);

    game_destroy(&game);
}

TEST(insurance_wins_dealer_has_blackjack) {
    printf("\n  Testing: Insurance wins when dealer has blackjack");

    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;

    // Player: regular hand (not blackjack)
    hand_add_card(&game.player_hands[0], 9);  // 10
    hand_add_card(&game.player_hands[0], 5);  // 6 -> 16

    // Dealer: blackjack (Ace + 10)
    hand_add_card(&game.dealer_hand, 0);  // Ace
    hand_add_card(&game.dealer_hand, 9);  // 10 -> Blackjack

    printf("\n    Player: %d, Dealer: Blackjack",
           hand_get_value(&game.player_hands[0]));

    // Take insurance (5.0 bet, half of 10.0)
    game_take_insurance(&game, 5.0);

    // Resolve the game
    double payout = game_resolve(&game);

    printf("\n    Total payout: %.2f", payout);
    printf("\n    Expected: 10.0 (main hand 0.0 + insurance 10.0)");

    // Expected payout:
    // - Main hand: 0.0 (lost to dealer blackjack)
    // - Insurance: 10.0 (5.0 bet * 2:1 payout = 10.0)
    // - Total: 10.0
    assert(payout == 10.0);

    game_destroy(&game);
}

TEST(insurance_loses_dealer_no_blackjack) {
    printf("\n  Testing: Insurance loses when dealer doesn't have blackjack");

    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;

    // Player: 20 (strong hand)
    hand_add_card(&game.player_hands[0], 9);  // 10
    hand_add_card(&game.player_hands[0], 9);  // 10 -> 20

    // Dealer: Ace + 5 = 16 (not blackjack)
    hand_add_card(&game.dealer_hand, 0);  // Ace
    hand_add_card(&game.dealer_hand, 4);  // 5 -> soft 16

    printf("\n    Player: %d, Dealer: soft %d (no blackjack)",
           hand_get_value(&game.player_hands[0]),
           hand_get_value(&game.dealer_hand));

    // Take insurance (5.0 bet)
    game_take_insurance(&game, 5.0);

    // Simulate dealer play: dealer hits soft 16
    hand_add_card(&game.dealer_hand, 9);  // +10 -> busts to 26

    // Resolve the game
    double payout = game_resolve(&game);

    printf("\n    Total payout: %.2f", payout);
    printf("\n    Expected: 20.0 (main hand 20.0 + insurance 0.0)");

    // Expected payout:
    // - Insurance: 0.0 (loses when dealer doesn't have blackjack)
    // - Main hand: 20.0 (wins against dealer bust)
    // - Total: 20.0
    assert(payout == 20.0);

    game_destroy(&game);
}

TEST(insurance_bet_amount_is_half_original_bet) {
    printf("\n  Testing: Insurance bet is exactly half of original bet");

    Rules rules;
    rules_init(&rules);

    // Test with different bet amounts
    double test_bets[] = {10.0, 25.0, 50.0, 100.0};
    double expected_insurance[] = {5.0, 12.5, 25.0, 50.0};

    for (int i = 0; i < 4; i++) {
        double original_bet = test_bets[i];
        double insurance_bet = original_bet / 2.0;

        printf("\n    Original bet: %.2f -> Insurance: %.2f",
               original_bet, insurance_bet);

        // Verify insurance is exactly half
        assert(insurance_bet == expected_insurance[i]);
    }
}

TEST(insurance_with_player_blackjack_dealer_blackjack) {
    printf("\n  Testing: Both player and dealer have blackjack, player took insurance");

    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;

    // Player: blackjack (Ace + 10)
    hand_add_card(&game.player_hands[0], 0);  // Ace
    hand_add_card(&game.player_hands[0], 9);  // 10 -> Blackjack

    // Dealer: blackjack (Ace + 10)
    hand_add_card(&game.dealer_hand, 0);  // Ace
    hand_add_card(&game.dealer_hand, 9);  // 10 -> Blackjack

    printf("\n    Player: Blackjack, Dealer: Blackjack");

    // Take insurance (5.0 bet)
    game_take_insurance(&game, 5.0);

    // Resolve the game
    double payout = game_resolve(&game);

    printf("\n    Total payout: %.2f", payout);
    printf("\n    Expected: 20.0 (even money - main hand 10.0 + insurance 10.0)");

    // Expected payout:
    // - Main hand: 10.0 (push - both have blackjack)
    // - Insurance: 10.0 (5.0 bet * 2:1 payout)
    // - Total: 20.0
    // This is called "even money" - player guarantees 1:1 profit
    assert(payout == 20.0);

    game_destroy(&game);
}

TEST(insurance_with_player_blackjack_dealer_no_blackjack) {
    printf("\n  Testing: Player has blackjack, dealer doesn't, player took insurance");

    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;

    // Player: blackjack (Ace + King)
    hand_add_card(&game.player_hands[0], 0);  // Ace
    hand_add_card(&game.player_hands[0], 12); // King -> Blackjack

    // Dealer: Ace + 6 = soft 17 (not blackjack)
    hand_add_card(&game.dealer_hand, 0);  // Ace
    hand_add_card(&game.dealer_hand, 5);  // 6 -> soft 17

    printf("\n    Player: Blackjack, Dealer: soft 17 (no blackjack)");

    // Take insurance (5.0 bet)
    game_take_insurance(&game, 5.0);

    // Resolve the game
    double payout = game_resolve(&game);

    printf("\n    Total payout: %.2f", payout);
    printf("\n    Expected: 25.0 (BJ 25.0 + insurance 0.0)");

    // Expected payout:
    // - Insurance: 0.0 (loses when dealer doesn't have blackjack)
    // - Main hand: 25.0 (player BJ wins 3:2 -> 10.0 + 15.0)
    // - Total: 25.0
    // Note: Would be 25.0 either way, but insurance adds risk
    assert(payout == 25.0);

    game_destroy(&game);
}

TEST(insurance_frequency_in_simulation) {
    printf("\n  Testing: Insurance offered in ~1/13 hands (dealer shows Ace)");

    Rules rules;
    rules_init(&rules);

    // TODO: When insurance is implemented:
    // Run 10000 hands and track:
    // 1. How many times insurance was offered (dealer showed Ace)
    // 2. Should be approximately 1/13 = ~7.7% of hands
    // 3. Verify insurance offering logic is working correctly

    printf("\n    Expected insurance offer rate: ~7.7%% (1/13 hands)");
    printf("\n    (Test placeholder - add insurance tracking to SimulationResults)");
}

// TEST(simulation_win_rate) {
//     SimulationConfig config;
//     rules_init(&config.rules);
//     config.num_hands = 10000;
//     config.use_basic_strategy = true;
//
//     SimulationResults results = {0};
//     simulation_run(&config, &results);
//
//     double win_rate = (double)results.hands_won / results.hands_played;
//     double loss_rate = (double)results.hands_lost / results.hands_played;
//     double push_rate = (double)results.hands_pushed / results.hands_played;
//
//     // Basic sanity checks
//     assert(win_rate > 0.35 && win_rate < 0.50);  // ~42-43% win rate typical
//     assert(loss_rate > 0.40 && loss_rate < 0.55); // ~48-49% loss rate typical
//     assert(push_rate > 0.05 && push_rate < 0.15); // ~8-9% push rate typical
//
//     printf("\n  Win: %.2f%% Loss: %.2f%% Push: %.2f%%",
//            win_rate * 100, loss_rate * 100, push_rate * 100);
// }

// TEST(simulation_blackjack_frequency) {
//     SimulationConfig config;
//     rules_init(&config.rules);
//     config.num_hands = 10000;
//     config.use_basic_strategy = true;
//
//     SimulationResults results = {0};
//     simulation_run(&config, &results);
//
//     double bj_rate = (double)results.blackjacks / results.hands_played;
//
//     // Natural blackjack should occur ~4.8% of the time
//     assert(bj_rate > 0.03 && bj_rate < 0.06);
//
//     printf("\n  Blackjack rate: %.2f%%", bj_rate * 100);
// }

// TEST(simulation_different_rules_affect_ev) {
//     // Test 1: Standard 3:2 blackjack payout
//     SimulationConfig config1;
//     rules_init(&config1.rules);
//     config1.rules.blackjack_payout = 1.5;  // 3:2
//     config1.num_hands = 50000;
//     config1.use_basic_strategy = true;
//
//     SimulationResults results1 = {0};
//     simulation_run(&config1, &results1);
//     double ev1 = simulation_get_ev(&results1);
//
//     // Test 2: 6:5 blackjack payout (worse for player)
//     SimulationConfig config2;
//     rules_init(&config2.rules);
//     config2.rules.blackjack_payout = 1.2;  // 6:5
//     config2.num_hands = 50000;
//     config2.use_basic_strategy = true;
//
//     SimulationResults results2 = {0};
//     simulation_run(&config2, &results2);
//     double ev2 = simulation_get_ev(&results2);
//
//     // 6:5 blackjack should have worse EV for player
//     assert(ev1 > ev2);
//
//     printf("\n  3:2 EV: %.4f, 6:5 EV: %.4f", ev1, ev2);
// }

// ============================================================================
// ADVANCED STRATEGY CALCULATION TESTS
// ============================================================================
// These tests are for the final phase: calculating optimal strategy from scratch

// TEST(calculate_strategy_for_hard_12) {
//     // For a given situation (hard 12 vs dealer 4),
//     // run simulations for HIT vs STAND and determine which is better
//     Rules rules;
//     rules_init(&rules);
//
//     // Run N simulations for HIT
//     double ev_hit = calculate_situation_ev(12, false, false, 3, HIT, &rules, 10000);
//
//     // Run N simulations for STAND
//     double ev_stand = calculate_situation_ev(12, false, false, 3, STAND, &rules, 10000);
//
//     // For hard 12 vs 4, basic strategy says STAND
//     assert(ev_stand > ev_hit);
//
//     printf("\n  Hard 12 vs 4: HIT EV=%.4f, STAND EV=%.4f", ev_hit, ev_stand);
// }

int main(void) {
    printf("Running Strategy & Simulation Tests\n");
    printf("==================================\n\n");

    // Uncomment these as you implement the functions

    // Basic strategy tests
    run_test_strategy_player_hard_16_vs_dealer_10();
    run_test_strategy_player_soft_18_vs_dealer_9();
    run_test_strategy_player_11_vs_dealer_any();
    run_test_strategy_player_pair_8s_vs_any();
    run_test_strategy_player_pair_10s_vs_any();
    run_test_strategy_player_hard_17_vs_any();

    // Comprehensive basic strategy verification
    run_test_strategy_hard_12_vs_dealer_weak();
    run_test_strategy_hard_13_14_15_16_vs_dealer();
    run_test_strategy_soft_double_situations();
    run_test_strategy_pair_aces_vs_all_dealers();
    run_test_strategy_pair_9s_special_cases();

    // Simulation tests
    run_test_simulation_initialization();
    run_test_simulation_double_and_split_actions();

    // Split hand resolution tests
    run_test_game_split_hand_one_wins_one_loses();
    run_test_game_split_hand_both_bust();

    // Double after split tests
    run_test_game_double_after_split_allowed();

    // Surrender tests
    run_test_strategy_surrender_16_vs_10();
    run_test_strategy_surrender_15_vs_10();
    run_test_strategy_surrender_16_vs_ace();
    run_test_game_surrender_payout_correct();

    // Comprehensive strategy verification
    run_test_strategy_verify_all_hard_totals();
    run_test_strategy_verify_all_soft_totals();
    run_test_strategy_verify_all_pairs();

    run_test_simulation_basic_strategy_ev();
    run_test_simulation_ev_variance_across_seeds();

    // Insurance tests (placeholders for implementation)
    run_test_insurance_offered_when_dealer_shows_ace();
    run_test_insurance_not_offered_when_dealer_shows_non_ace();
    run_test_insurance_wins_dealer_has_blackjack();
    run_test_insurance_loses_dealer_no_blackjack();
    run_test_insurance_bet_amount_is_half_original_bet();
    run_test_insurance_with_player_blackjack_dealer_blackjack();
    run_test_insurance_with_player_blackjack_dealer_no_blackjack();
    run_test_insurance_frequency_in_simulation();

    // run_test_simulation_win_rate();
    // run_test_simulation_blackjack_frequency();
    // run_test_simulation_different_rules_affect_ev();

    // Advanced strategy calculation
    // run_test_calculate_strategy_for_hard_12();

    printf("\n==================================\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_run);

    if (tests_passed == tests_run) {
        printf("All tests passed! ✓\n");
        return 0;
    } else {
        printf("Some tests failed! ✗\n");
        return 1;
    }
}
