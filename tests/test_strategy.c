#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
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
        PlayerAction action = get_basic_strategy_action(&player, dealer_upcard, &rules, &strategy, false, false);
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
    PlayerAction action = get_basic_strategy_action(&player, dealer_upcard, &rules, &strategy, false, true);
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
        PlayerAction action = get_basic_strategy_action(&player, dealer_upcard, &rules, &strategy, false, true);
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
        PlayerAction action = get_basic_strategy_action(&player, dealer_upcard, &rules, &strategy, true, true);
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
        PlayerAction action = get_basic_strategy_action(&player, dealer_upcard, &rules, &strategy, true, true);
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
        PlayerAction action = get_basic_strategy_action(&player, dealer_upcard, &rules, &strategy, false, false);
        assert(action == STAND);
    }

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

// TEST(simulation_basic_strategy_ev) {
//     SimulationConfig config;
//     rules_init(&config.rules);
//     config.num_hands = 100000;  // Large sample for accuracy
//     config.use_basic_strategy = true;
//
//     SimulationResults results = {0};
//     simulation_run(&config, &results);
//
//     double ev = simulation_get_ev(&results);
//
//     // With perfect basic strategy and standard rules,
//     // house edge should be around 0.5% (EV around -0.005)
//     // Allow some variance: -0.01 to 0.0
//     assert(ev > -0.01 && ev < 0.0);
//
//     printf("\n  Expected Value: %.4f (%.2f%%)", ev, ev * 100);
// }

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

    // Simulation tests
    run_test_simulation_initialization();
    run_test_simulation_double_and_split_actions();
    // run_test_simulation_basic_strategy_ev();
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
