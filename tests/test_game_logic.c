#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
// Uncomment these as you create the headers
#include "../src/dealer.h"
#include "../src/game.h"

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
// DEALER LOGIC TESTS
// ============================================================================
// Implement dealer.h with:
// - bool dealer_should_hit(Hand* dealer_hand, GameRules* rules)
// - void dealer_play(Hand* dealer_hand, Deck* deck, GameRules* rules)

TEST(dealer_hits_on_16) {
    Hand hand;
    hand_init(&hand);
    hand_add_card(&hand, 9);   // 10
    hand_add_card(&hand, 5);   // 6

    Rules rules;
    rules_init(&rules);

    assert(dealer_should_hit(&hand, &rules) == true);

    hand_destroy(&hand);
}

TEST(dealer_stands_on_17) {
    Hand hand;
    hand_init(&hand);
    hand_add_card(&hand, 9);   // 10
    hand_add_card(&hand, 6);   // 7

    Rules rules;
    rules_init(&rules);

    assert(dealer_should_hit(&hand, &rules) == false);

    hand_destroy(&hand);
}

TEST(dealer_stands_on_soft_17_standard) {
    Hand hand;
    hand_init(&hand);
    hand_add_card(&hand, 0);   // Ace
    hand_add_card(&hand, 5);   // 6

    Rules rules;
    rules_init(&rules);
    rules.dealer_hits_soft_17 = false;

    assert(dealer_should_hit(&hand, &rules) == false);

    hand_destroy(&hand);
}

TEST(dealer_hits_on_soft_17_h17) {
    Hand hand;
    hand_init(&hand);
    hand_add_card(&hand, 0);   // Ace
    hand_add_card(&hand, 5);   // 6

    Rules rules;
    rules_init(&rules);
    rules.dealer_hits_soft_17 = true;

    assert(dealer_should_hit(&hand, &rules) == true);

    hand_destroy(&hand);
}

// ============================================================================
// GAME STATE TESTS
// ============================================================================
// Implement game.h with:
// typedef enum { HIT, STAND, DOUBLE, SPLIT, SURRENDER } PlayerAction;
//
// typedef struct {
//     Deck deck;
//     Hand dealer_hand;
//     Hand player_hand;
//     // For splits: Hand* split_hands; int num_split_hands;
//     GameRules rules;
//     double bet;
//     bool game_over;
// } GameState;
//
// void game_init(GameState* game, GameRules* rules, double initial_bet)
// void game_destroy(GameState* game)
// void game_deal_initial(GameState* game)
// bool game_play_action(GameState* game, PlayerAction action)
// double game_resolve(GameState* game) - Returns payout multiplier

TEST(game_initialization) {
    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);

    assert(game.player_bets[0] == 10.0);
    assert(game.game_over == false);
    assert(game.deck.num_decks == rules.num_decks);

    game_destroy(&game);
}

TEST(game_initial_deal) {
    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game_deal_initial(&game);

    // Player should have 2 cards
    assert(game.player_hands[0].num_cards == 2);

    // Dealer should have 2 cards
    assert(game.dealer_hand.num_cards == 2);

    // 4 cards should be dealt from deck
    assert(game.deck.position == 4);

    game_destroy(&game);
}

TEST(game_player_hits) {
    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game_deal_initial(&game);

    int initial_cards = game.player_hands[0].num_cards;
    game_play_action(&game, HIT, 0);

    assert(game.player_hands[0].num_cards == initial_cards + 1);

    game_destroy(&game);
}

TEST(game_player_stands) {
    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game_deal_initial(&game);

    int player_cards = game.player_hands[0].num_cards;
    game_play_action(&game, STAND, 0);

    // Player hand should not change
    assert(game.player_hands[0].num_cards == player_cards);

    // STAND action doesn't trigger dealer play - that's handled by simulation
    // Dealer should still have initial 2 cards
    assert(game.dealer_hand.num_cards == 2);

    game_destroy(&game);
}

TEST(game_player_busts) {
    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;  // Set up manually instead of dealing

    // Force a bust scenario: give player 10, 10, then hit with 10
    hand_add_card(&game.player_hands[0], 9);   // 10
    hand_add_card(&game.player_hands[0], 9);   // 10
    hand_add_card(&game.dealer_hand, 9);   // Dealer 10
    hand_add_card(&game.dealer_hand, 5);   // Dealer 6

    game_play_action(&game, HIT, 0);
    // After hitting, if we get high card, we bust

    if (hand_get_value(&game.player_hands[0]) > 21) {
        double payout = game_resolve(&game);
        assert(payout == 0.0);  // Lose bet - total payout is 0
    }

    game_destroy(&game);
}

TEST(game_blackjack_payout) {
    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;  // Set up manually

    // Give player blackjack
    hand_add_card(&game.player_hands[0], 0);   // Ace
    hand_add_card(&game.player_hands[0], 9);   // 10

    // Give dealer non-blackjack
    hand_add_card(&game.dealer_hand, 9);   // 10
    hand_add_card(&game.dealer_hand, 6);   // 7

    game_play_action(&game, STAND, 0);
    double payout = game_resolve(&game);

    // Should get 2.5x bet (original + 1.5x winnings) = 10 * 2.5 = 25.0
    assert(payout == 25.0);

    game_destroy(&game);
}

TEST(game_player_wins) {
    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;  // Set up manually

    // Player: 20
    hand_add_card(&game.player_hands[0], 9);   // 10
    hand_add_card(&game.player_hands[0], 9);   // 10

    // Dealer: 19
    hand_add_card(&game.dealer_hand, 9);   // 10
    hand_add_card(&game.dealer_hand, 8);   // 9

    game_play_action(&game, STAND, 0);
    double payout = game_resolve(&game);

    // Should get 2x bet (original + 1x winnings) = 10 * 2.0 = 20.0
    assert(payout == 20.0);

    game_destroy(&game);
}

TEST(game_push) {
    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;  // Set up manually

    // Both have 20
    hand_add_card(&game.player_hands[0], 9);   // 10
    hand_add_card(&game.player_hands[0], 9);   // 10
    hand_add_card(&game.dealer_hand, 9);   // 10
    hand_add_card(&game.dealer_hand, 9);   // 10

    game_play_action(&game, STAND, 0);
    double payout = game_resolve(&game);

    // Push - get bet back = 10 * 1.0 = 10.0
    assert(payout == 10.0);

    game_destroy(&game);
}

TEST(game_dealer_busts) {
    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;  // Set up manually

    // Player: 18
    hand_add_card(&game.player_hands[0], 9);   // 10
    hand_add_card(&game.player_hands[0], 7);   // 8

    // Dealer busts with 22
    hand_add_card(&game.dealer_hand, 9);   // 10
    hand_add_card(&game.dealer_hand, 5);   // 6
    hand_add_card(&game.dealer_hand, 5);   // 6 -> 22

    game_play_action(&game, STAND, 0);

    double payout = game_resolve(&game);
    assert(payout == 20.0);  // Player wins: 10 * 2.0 = 20.0

    game_destroy(&game);
}

TEST(game_twentyone_not_blackjack) {
    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;  // Set up manually

    // Player: 21 with 3 cards (7+7+7) - NOT a blackjack
    hand_add_card(&game.player_hands[0], 6);   // 7
    hand_add_card(&game.player_hands[0], 6);   // 7
    hand_add_card(&game.player_hands[0], 6);   // 7

    // Dealer: 20
    hand_add_card(&game.dealer_hand, 9);   // 10
    hand_add_card(&game.dealer_hand, 9);   // 10

    game_play_action(&game, STAND, 0);
    double payout = game_resolve(&game);

    // Should get 2.0x (regular win), NOT 2.5x (blackjack payout) = 10 * 2.0 = 20.0
    assert(payout == 20.0);

    game_destroy(&game);
}

// ============================================================================
// DOUBLE DOWN TESTS
// ============================================================================

TEST(game_player_doubles_down) {
    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;  // Set up manually

    // Player: 11 (good double situation)
    hand_add_card(&game.player_hands[0], 5);   // 6
    hand_add_card(&game.player_hands[0], 4);   // 5

    // Dealer: some hand
    hand_add_card(&game.dealer_hand, 5);   // 6
    hand_add_card(&game.dealer_hand, 4);   // 5

    int initial_cards = game.player_hands[0].num_cards;
    double initial_bet = game.player_bets[0];

    game_play_action(&game, DOUBLE, 0);

    // Should have exactly one more card
    assert(game.player_hands[0].num_cards == initial_cards + 1);

    // Bet should be doubled
    assert(game.player_bets[0] == initial_bet * 2);

    game_destroy(&game);
}

TEST(game_double_down_wins) {
    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;  // Set up manually

    // Player: 11, will get 9 = 20
    hand_add_card(&game.player_hands[0], 5);   // 6
    hand_add_card(&game.player_hands[0], 4);   // 5

    // Dealer: 18
    hand_add_card(&game.dealer_hand, 9);   // 10
    hand_add_card(&game.dealer_hand, 7);   // 8

    game_play_action(&game, DOUBLE, 0);

    // Bet should now be 20.0
    assert(game.player_bets[0] == 20.0);

    // If player wins, payout should reflect doubled bet
    double payout = game_resolve(&game);

    // If player won, payout should be 2.0x the doubled bet = 20 * 2.0 = 40.0
    if (hand_get_value(&game.player_hands[0]) > hand_get_value(&game.dealer_hand) &&
        hand_get_value(&game.player_hands[0]) <= 21) {
        assert(payout == 40.0);
    }

    game_destroy(&game);
}

// ============================================================================
// SURRENDER TESTS
// ============================================================================

TEST(game_player_surrenders) {
    Rules rules;
    rules_init(&rules);
    rules.late_surrender_allowed = true;

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;  // Set up manually

    // Player: 16 (bad situation)
    hand_add_card(&game.player_hands[0], 9);   // 10
    hand_add_card(&game.player_hands[0], 5);   // 6

    // Dealer: 10
    hand_add_card(&game.dealer_hand, 9);   // 10
    hand_add_card(&game.dealer_hand, 6);   // 7

    assert(game.surrendered == false);

    game_play_action(&game, SURRENDER, 0);

    // Should set surrendered flag
    assert(game.surrendered == true);

    game_destroy(&game);
}

TEST(game_surrender_payout) {
    Rules rules;
    rules_init(&rules);
    rules.late_surrender_allowed = true;

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;  // Set up manually

    // Player: 16
    hand_add_card(&game.player_hands[0], 9);   // 10
    hand_add_card(&game.player_hands[0], 5);   // 6

    // Dealer: 10
    hand_add_card(&game.dealer_hand, 9);   // 10
    hand_add_card(&game.dealer_hand, 6);   // 7

    game_play_action(&game, SURRENDER, 0);
    double payout = game_resolve(&game);

    // Should get 0.5x bet back (half the bet) = 10 * 0.5 = 5.0
    assert(payout == 5.0);

    game_destroy(&game);
}

TEST(game_surrender_vs_losing) {
    Rules rules;
    rules_init(&rules);

    GameState game_surrender;
    game_init(&game_surrender, &rules, 10.0);
    game_surrender.num_player_hands = 1;

    // Player: 16 (will likely lose)
    hand_add_card(&game_surrender.player_hands[0], 9);   // 10
    hand_add_card(&game_surrender.player_hands[0], 5);   // 6

    // Dealer: 20 (strong hand)
    hand_add_card(&game_surrender.dealer_hand, 9);   // 10
    hand_add_card(&game_surrender.dealer_hand, 9);   // 10

    game_play_action(&game_surrender, SURRENDER, 0);
    double surrender_payout = game_resolve(&game_surrender);

    // Surrender gets 5.0 back
    assert(surrender_payout == 5.0);

    // Compare to playing it out and losing
    GameState game_lose;
    game_init(&game_lose, &rules, 10.0);
    game_lose.num_player_hands = 1;

    hand_add_card(&game_lose.player_hands[0], 9);   // 10
    hand_add_card(&game_lose.player_hands[0], 5);   // 6
    hand_add_card(&game_lose.dealer_hand, 9);   // 10
    hand_add_card(&game_lose.dealer_hand, 9);   // 10

    game_play_action(&game_lose, STAND, 0);
    double lose_payout = game_resolve(&game_lose);

    // Standing and losing gets 0.0 back
    assert(lose_payout == 0.0);

    // Surrender is better than losing
    assert(surrender_payout > lose_payout);

    game_destroy(&game_surrender);
    game_destroy(&game_lose);
}

// ============================================================================
// SPLIT TESTS
// ============================================================================

TEST(game_player_splits_pair) {
    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;  // Set up manually

    // Player: pair of 8s
    hand_add_card(&game.player_hands[0], 7);   // 8
    hand_add_card(&game.player_hands[0], 7);   // 8

    // Dealer: some hand
    hand_add_card(&game.dealer_hand, 5);   // 6
    hand_add_card(&game.dealer_hand, 4);   // 5

    assert(hand_can_split(&game.player_hands[0]) == true);

    game_play_action(&game, SPLIT, 0);

    // Should now have 2 hands
    assert(game.num_player_hands == 2);

    // Each split hand should have 2 cards (1 original + 1 new card)
    assert(game.player_hands[0].num_cards == 2);
    assert(game.player_hands[1].num_cards == 2);

    // Both bets should be equal
    assert(game.player_bets[0] == 10.0);
    assert(game.player_bets[1] == 10.0);

    game_destroy(&game);
}

TEST(game_split_aces) {
    Rules rules;
    rules_init(&rules);

    GameState game;
    game_init(&game, &rules, 10.0);
    game.num_player_hands = 1;  // Set up manually

    // Player: pair of Aces
    hand_add_card(&game.player_hands[0], 0);   // Ace
    hand_add_card(&game.player_hands[0], 0);   // Ace

    // Dealer: some hand
    hand_add_card(&game.dealer_hand, 9);   // 10
    hand_add_card(&game.dealer_hand, 6);   // 7

    game_play_action(&game, SPLIT, 0);

    // Split aces get one card each (2 cards total per hand)
    assert(game.num_player_hands == 2);
    assert(game.player_hands[0].num_cards == 2);
    assert(game.player_hands[1].num_cards == 2);

    // Each hand should start with an Ace
    assert(game.player_hands[0].cards[0] == 0);  // Ace
    assert(game.player_hands[1].cards[0] == 0);  // Ace

    game_destroy(&game);
}

int main(void) {
    printf("Running Game Logic Tests\n");
    printf("==================================\n\n");

    // Uncomment these as you implement the functions

    // Dealer tests
    run_test_dealer_hits_on_16();
    run_test_dealer_stands_on_17();
    run_test_dealer_stands_on_soft_17_standard();
    run_test_dealer_hits_on_soft_17_h17();

    // Game state tests
    run_test_game_initialization();
    run_test_game_initial_deal();
    run_test_game_player_hits();
    run_test_game_player_stands();
    run_test_game_player_busts();
    run_test_game_blackjack_payout();
    run_test_game_player_wins();
    run_test_game_push();
    run_test_game_dealer_busts();
    run_test_game_twentyone_not_blackjack();

    // Double down tests
    run_test_game_player_doubles_down();
    run_test_game_double_down_wins();

    // Surrender tests
    run_test_game_player_surrenders();
    run_test_game_surrender_payout();
    run_test_game_surrender_vs_losing();

    // Split tests
    run_test_game_player_splits_pair();
    run_test_game_split_aces();

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
