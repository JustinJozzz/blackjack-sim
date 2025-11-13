#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
// #include "game.h"
#include "deck.h"

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

TEST(deck_initialization) {
    Deck deck;
    deck_init(&deck, 6);
    
    assert(deck.num_decks == 6);
    assert(deck.total_cards == 312);
    assert(deck.position == 0);
    assert(deck.cards[0] == 0);
    assert(deck.cards[1] == 1);

    deck_destroy(&deck);
}

TEST(deck_shuffle) {
    Deck deck;
    deck_init(&deck, 1);
    deck_shuffle(&deck);
    
    // After shuffle, position should be reset
    assert(deck.position == 0);
    
    // Check that we still have all 52 cards
    assert(deck.total_cards == 52);

    deck_destroy(&deck);
}

TEST(card_dealing) {
    Deck deck;
    deck_init(&deck, 3);
    deck_shuffle(&deck);
    
    int first_card = deck_deal(&deck);
    assert(deck.position == 1);
    
    int second_card = deck_deal(&deck);
    assert(deck.position == 2);
    assert(first_card != second_card);

    deck_destroy(&deck);
}

// Add more tests here as you implement features:
// TEST(hand_value_calculation) { ... }
// TEST(blackjack_detection) { ... }
// TEST(dealer_logic) { ... }
// TEST(basic_strategy) { ... }

int main(void) {
    printf("Running Blackjack Simulator Tests\n");
    printf("==================================\n\n");
    
    // Run all tests
    run_test_deck_initialization();
    run_test_deck_shuffle();
    run_test_card_dealing();
    
    // Add more test runs here as you create them
    // run_test_hand_value_calculation();
    // run_test_blackjack_detection();
    
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