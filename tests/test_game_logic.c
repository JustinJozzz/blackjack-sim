#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
// Uncomment these as you create the headers
// #include "../src/rules.h"
// #include "../src/dealer.h"
// #include "../src/game.h"
// #include "../src/hand.h"
// #include "../src/card.h"

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
// GAME RULES TESTS
// ============================================================================
// Implement rules.h with:
// typedef struct {
//     bool dealer_hits_soft_17;
//     bool double_after_split;
//     bool surrender_allowed;
//     int max_splits;
//     double blackjack_payout;  // 1.5 for 3:2, 1.2 for 6:5
//     bool resplit_aces;
//     int num_decks;
// } GameRules;
//
// void rules_init_standard(GameRules* rules) - Standard Vegas rules
// void rules_init_custom(GameRules* rules, ...) - Custom configuration

// TEST(standard_rules_initialization) {
//     GameRules rules;
//     rules_init_standard(&rules);
//
//     assert(rules.dealer_hits_soft_17 == false);  // Dealer stands on soft 17
//     assert(rules.double_after_split == true);
//     assert(rules.surrender_allowed == true);
//     assert(rules.max_splits == 3);
//     assert(rules.blackjack_payout == 1.5);  // 3:2
//     assert(rules.resplit_aces == false);
//     assert(rules.num_decks == 6);
// }

// ============================================================================
// DEALER LOGIC TESTS
// ============================================================================
// Implement dealer.h with:
// - bool dealer_should_hit(Hand* dealer_hand, GameRules* rules)
// - void dealer_play(Hand* dealer_hand, Deck* deck, GameRules* rules)

// TEST(dealer_hits_on_16) {
//     Hand hand;
//     hand_init(&hand);
//     hand_add_card(&hand, 9);   // 10
//     hand_add_card(&hand, 5);   // 6
//
//     GameRules rules;
//     rules_init_standard(&rules);
//
//     assert(dealer_should_hit(&hand, &rules) == true);
//
//     hand_destroy(&hand);
// }

// TEST(dealer_stands_on_17) {
//     Hand hand;
//     hand_init(&hand);
//     hand_add_card(&hand, 9);   // 10
//     hand_add_card(&hand, 6);   // 7
//
//     GameRules rules;
//     rules_init_standard(&rules);
//
//     assert(dealer_should_hit(&hand, &rules) == false);
//
//     hand_destroy(&hand);
// }

// TEST(dealer_stands_on_soft_17_standard) {
//     Hand hand;
//     hand_init(&hand);
//     hand_add_card(&hand, 0);   // Ace
//     hand_add_card(&hand, 5);   // 6
//
//     GameRules rules;
//     rules_init_standard(&rules);
//     rules.dealer_hits_soft_17 = false;
//
//     assert(dealer_should_hit(&hand, &rules) == false);
//
//     hand_destroy(&hand);
// }

// TEST(dealer_hits_on_soft_17_h17) {
//     Hand hand;
//     hand_init(&hand);
//     hand_add_card(&hand, 0);   // Ace
//     hand_add_card(&hand, 5);   // 6
//
//     GameRules rules;
//     rules_init_standard(&rules);
//     rules.dealer_hits_soft_17 = true;
//
//     assert(dealer_should_hit(&hand, &rules) == true);
//
//     hand_destroy(&hand);
// }

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

// TEST(game_initialization) {
//     GameRules rules;
//     rules_init_standard(&rules);
//
//     GameState game;
//     game_init(&game, &rules, 10.0);
//
//     assert(game.bet == 10.0);
//     assert(game.game_over == false);
//     assert(game.deck.num_decks == rules.num_decks);
//
//     game_destroy(&game);
// }

// TEST(game_initial_deal) {
//     GameRules rules;
//     rules_init_standard(&rules);
//
//     GameState game;
//     game_init(&game, &rules, 10.0);
//     game_deal_initial(&game);
//
//     // Player should have 2 cards
//     assert(game.player_hand.num_cards == 2);
//
//     // Dealer should have 2 cards
//     assert(game.dealer_hand.num_cards == 2);
//
//     // 4 cards should be dealt from deck
//     assert(game.deck.position == 4);
//
//     game_destroy(&game);
// }

// TEST(game_player_hits) {
//     GameRules rules;
//     rules_init_standard(&rules);
//
//     GameState game;
//     game_init(&game, &rules, 10.0);
//     game_deal_initial(&game);
//
//     int initial_cards = game.player_hand.num_cards;
//     game_play_action(&game, HIT);
//
//     assert(game.player_hand.num_cards == initial_cards + 1);
//
//     game_destroy(&game);
// }

// TEST(game_player_stands) {
//     GameRules rules;
//     rules_init_standard(&rules);
//
//     GameState game;
//     game_init(&game, &rules, 10.0);
//     game_deal_initial(&game);
//
//     int player_cards = game.player_hand.num_cards;
//     game_play_action(&game, STAND);
//
//     // Player hand should not change
//     assert(game.player_hand.num_cards == player_cards);
//
//     // After stand, dealer should play (will have more than 2 cards unless 17+)
//     int dealer_value = hand_get_value(&game.dealer_hand);
//     assert(dealer_value >= 17 || dealer_value == 0); // 0 if busted
//
//     game_destroy(&game);
// }

// TEST(game_player_busts) {
//     GameRules rules;
//     rules_init_standard(&rules);
//
//     GameState game;
//     game_init(&game, &rules, 10.0);
//
//     // Force a bust scenario: give player 10, 10, then hit with 10
//     hand_add_card(&game.player_hand, 9);   // 10
//     hand_add_card(&game.player_hand, 9);   // 10
//     hand_add_card(&game.dealer_hand, 9);   // Dealer 10
//     hand_add_card(&game.dealer_hand, 5);   // Dealer 6
//
//     game_play_action(&game, HIT);
//     // After hitting, if we get high card, we bust
//
//     if (hand_get_value(&game.player_hand) > 21) {
//         double payout = game_resolve(&game);
//         assert(payout == 0.0);  // Lose bet
//     }
//
//     game_destroy(&game);
// }

// TEST(game_blackjack_payout) {
//     GameRules rules;
//     rules_init_standard(&rules);
//
//     GameState game;
//     game_init(&game, &rules, 10.0);
//
//     // Give player blackjack
//     hand_add_card(&game.player_hand, 0);   // Ace
//     hand_add_card(&game.player_hand, 9);   // 10
//
//     // Give dealer non-blackjack
//     hand_add_card(&game.dealer_hand, 9);   // 10
//     hand_add_card(&game.dealer_hand, 6);   // 7
//
//     game_play_action(&game, STAND);
//     double payout = game_resolve(&game);
//
//     // Should get 2.5x bet (original + 1.5x winnings)
//     assert(payout == 2.5);
//
//     game_destroy(&game);
// }

// TEST(game_player_wins) {
//     GameRules rules;
//     rules_init_standard(&rules);
//
//     GameState game;
//     game_init(&game, &rules, 10.0);
//
//     // Player: 20
//     hand_add_card(&game.player_hand, 9);   // 10
//     hand_add_card(&game.player_hand, 9);   // 10
//
//     // Dealer: 19
//     hand_add_card(&game.dealer_hand, 9);   // 10
//     hand_add_card(&game.dealer_hand, 8);   // 9
//
//     game_play_action(&game, STAND);
//     double payout = game_resolve(&game);
//
//     // Should get 2x bet (original + 1x winnings)
//     assert(payout == 2.0);
//
//     game_destroy(&game);
// }

// TEST(game_push) {
//     GameRules rules;
//     rules_init_standard(&rules);
//
//     GameState game;
//     game_init(&game, &rules, 10.0);
//
//     // Both have 20
//     hand_add_card(&game.player_hand, 9);   // 10
//     hand_add_card(&game.player_hand, 9);   // 10
//     hand_add_card(&game.dealer_hand, 9);   // 10
//     hand_add_card(&game.dealer_hand, 9);   // 10
//
//     game_play_action(&game, STAND);
//     double payout = game_resolve(&game);
//
//     // Push - get bet back
//     assert(payout == 1.0);
//
//     game_destroy(&game);
// }

// TEST(game_dealer_busts) {
//     GameRules rules;
//     rules_init_standard(&rules);
//
//     GameState game;
//     game_init(&game, &rules, 10.0);
//
//     // Player: 18
//     hand_add_card(&game.player_hand, 9);   // 10
//     hand_add_card(&game.player_hand, 7);   // 8
//
//     // Dealer will bust (10 + 6 + high card)
//     hand_add_card(&game.dealer_hand, 9);   // 10
//     hand_add_card(&game.dealer_hand, 5);   // 6
//
//     game_play_action(&game, STAND);
//
//     // Dealer must draw and will likely bust
//     if (hand_get_value(&game.dealer_hand) > 21) {
//         double payout = game_resolve(&game);
//         assert(payout == 2.0);  // Player wins
//     }
//
//     game_destroy(&game);
// }

int main(void) {
    printf("Running Game Logic Tests\n");
    printf("==================================\n\n");

    // Uncomment these as you implement the functions

    // Rules tests
    // run_test_standard_rules_initialization();

    // Dealer tests
    // run_test_dealer_hits_on_16();
    // run_test_dealer_stands_on_17();
    // run_test_dealer_stands_on_soft_17_standard();
    // run_test_dealer_hits_on_soft_17_h17();

    // Game state tests
    // run_test_game_initialization();
    // run_test_game_initial_deal();
    // run_test_game_player_hits();
    // run_test_game_player_stands();
    // run_test_game_player_busts();
    // run_test_game_blackjack_payout();
    // run_test_game_player_wins();
    // run_test_game_push();
    // run_test_game_dealer_busts();

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
