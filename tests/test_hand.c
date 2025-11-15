#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "../src/card.h"
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
// CARD UTILITY TESTS
// ============================================================================
// Implement card.h with these functions:
// - int card_value(int card) - Returns blackjack value (1-11)
// - int card_rank(int card) - Returns rank 0-12 (0=A, 1=2, ... 9=10, 10=J, 11=Q, 12=K)
// - int card_suit(int card) - Returns suit 0-3 (0-3 for different suits)
// - const char* card_to_string(int card) - Returns string like "A♠", "10♥", etc.

TEST(card_values) {
    // Ace should return 11 (hand logic will handle ace as 1)
    assert(card_value(0) == 11);  // Ace of first suit

    // Number cards 2-10
    assert(card_value(1) == 2);   // 2
    assert(card_value(8) == 9);   // 9
    assert(card_value(9) == 10);  // 10

    // Face cards should all be 10
    assert(card_value(10) == 10); // Jack
    assert(card_value(11) == 10); // Queen
    assert(card_value(12) == 10); // King
}

TEST(card_ranks) {
    // Cards 0-12 are first suit
    assert(card_rank(0) == 0);   // Ace
    assert(card_rank(1) == 1);   // 2
    assert(card_rank(9) == 9);   // 10
    assert(card_rank(10) == 10); // Jack
    assert(card_rank(11) == 11); // Queen
    assert(card_rank(12) == 12); // King

    // Cards 13-25 are second suit (should have same ranks)
    assert(card_rank(13) == 0);  // Ace of second suit
    assert(card_rank(22) == 9);  // 10 of second suit
    assert(card_rank(25) == 12); // King of second suit
}

TEST(card_suits) {
    // First 13 cards (0-12) should be suit 0
    assert(card_suit(0) == 0);   // Ace of spades
    assert(card_suit(12) == 0);  // King of spades

    // Next 13 cards (13-25) should be suit 1
    assert(card_suit(13) == 1);  // Ace of hearts
    assert(card_suit(25) == 1);  // King of hearts

    // Next 13 cards (26-38) should be suit 2
    assert(card_suit(26) == 2);  // Ace of diamonds
    assert(card_suit(38) == 2);  // King of diamonds

    // Last 13 cards (39-51) should be suit 3
    assert(card_suit(39) == 3);  // Ace of clubs
    assert(card_suit(51) == 3);  // King of clubs
}

TEST(card_to_string) {
    // Test all ranks in first suit (spades)
    const char* str;

    str = card_to_string(0);
    assert(strcmp(str, "Ace of Diamonds") == 0);  // Ace

    str = card_to_string(1);
    assert(strcmp(str, "Two of Diamonds") == 0);  // 2

    str = card_to_string(21);
    assert(strcmp(str, "Nine of Spades") == 0);  // 9

    str = card_to_string(22);
    assert(strcmp(str, "Ten of Spades") == 0);  // 10 (two characters)

    str = card_to_string(36);
    assert(strcmp(str, "Jack of Hearts") == 0);  // Jack

    str = card_to_string(37);
    assert(strcmp(str, "Queen of Hearts") == 0);  // Queen

    str = card_to_string(51);
    assert(strcmp(str, "King of Clubs") == 0);  // King

    // Just verify we can convert any card without crashing
    for (int i = 0; i < 52; i++) {
        str = card_to_string(i);
        assert(str != NULL);
    }
}

// ============================================================================
// HAND TESTS
// ============================================================================
// Implement hand.h with:
// - typedef struct { int* cards; int num_cards; int capacity; } Hand;
// - void hand_init(Hand* hand)
// - void hand_destroy(Hand* hand)
// - void hand_add_card(Hand* hand, int card)
// - int hand_get_value(Hand* hand) - Returns best value (handles aces as 1 or 11)
// - bool hand_is_soft(Hand* hand) - True if contains ace counted as 11
// - bool hand_is_blackjack(Hand* hand) - True if natural 21 (ace + 10-value card)
// - bool hand_can_split(Hand* hand) - True if two cards of same rank
// - bool hand_can_double(Hand* hand) - True if hand has exactly 2 cards

TEST(hand_initialization) {
    Hand hand;
    hand_init(&hand);

    assert(hand.num_cards == 0);
    assert(hand.cards != NULL);
    assert(hand.capacity == 2);

    hand_destroy(&hand);
}

TEST(hand_add_cards) {
    Hand hand;
    hand_init(&hand);

    hand_add_card(&hand, 5);  // 6 of some suit
    assert(hand.num_cards == 1);
    assert(hand.cards[0] == 5);

    hand_add_card(&hand, 10); // Jack
    assert(hand.num_cards == 2);
    assert(hand.cards[1] == 10);

    // Test Realloc
    hand_add_card(&hand, 12); // King
    assert(hand.num_cards == 3);
    assert(hand.cards[2] == 12);

    hand_destroy(&hand);
}

TEST(hand_hard_values) {
    Hand hand;
    hand_init(&hand);

    // Hard 16 (10 + 6)
    hand_add_card(&hand, 9);  // 10
    hand_add_card(&hand, 5);  // 6
    assert(hand_get_value(&hand) == 16);
    assert(hand_is_soft(&hand) == false);

    hand_destroy(&hand);
}

TEST(hand_soft_values) {
    Hand hand;
    hand_init(&hand);

    // Soft 17 (Ace + 6)
    hand_add_card(&hand, 0);  // Ace
    hand_add_card(&hand, 5);  // 6
    assert(hand_get_value(&hand) == 17);
    assert(hand_is_soft(&hand) == true);

    hand_destroy(&hand);
}

TEST(hand_soft_becomes_hard) {
    Hand hand;
    hand_init(&hand);

    // Soft 17 becomes hard 17 when we add a 10
    hand_add_card(&hand, 0);  // Ace (11)
    hand_add_card(&hand, 5);  // 6
    assert(hand_get_value(&hand) == 17);
    assert(hand_is_soft(&hand) == true);

    hand_add_card(&hand, 9);  // 10
    // Now: A(1) + 6 + 10 = 17 hard
    assert(hand_get_value(&hand) == 17);
    assert(hand_is_soft(&hand) == false);

    hand_destroy(&hand);
}

TEST(hand_multiple_aces) {
    Hand hand;
    hand_init(&hand);

    // Two aces = 12 (soft)
    hand_add_card(&hand, 0);  // Ace
    hand_add_card(&hand, 0);  // Ace
    assert(hand_get_value(&hand) == 12);
    assert(hand_is_soft(&hand) == true);

    // Add 9 -> A(11) + A(1) + 9 = 21
    hand_add_card(&hand, 8);  // 9
    assert(hand_get_value(&hand) == 21);
    assert(hand_is_soft(&hand) == true);

    hand_destroy(&hand);
}

TEST(hand_bust_with_aces) {
    Hand hand;
    hand_init(&hand);

    // Ace + 6 + 5 + 5 = 17 (ace as 1)
    hand_add_card(&hand, 0);  // Ace
    hand_add_card(&hand, 5);  // 6
    hand_add_card(&hand, 4);  // 5
    hand_add_card(&hand, 4);  // 5
    assert(hand_get_value(&hand) == 17);
    assert(hand_is_soft(&hand) == false);

    hand_destroy(&hand);
}

TEST(hand_blackjack_detection) {
    Hand hand;
    hand_init(&hand);

    // Ace + 10 = blackjack
    hand_add_card(&hand, 0);  // Ace
    hand_add_card(&hand, 9);  // 10
    assert(hand_get_value(&hand) == 21);
    assert(hand_is_blackjack(&hand) == true);

    hand_destroy(&hand);

    // But 3 cards to 21 is not blackjack
    Hand hand2;
    hand_init(&hand2);
    hand_add_card(&hand2, 6);  // 7
    hand_add_card(&hand2, 6);  // 7
    hand_add_card(&hand2, 6);  // 7
    assert(hand_get_value(&hand2) == 21);
    assert(hand_is_blackjack(&hand2) == false);

    hand_destroy(&hand2);
}

TEST(hand_split_detection) {
    Hand hand;
    hand_init(&hand);

    // Two cards of same rank can split
    hand_add_card(&hand, 8);  // 9
    hand_add_card(&hand, 8);  // 9
    assert(hand_can_split(&hand) == true);

    hand_destroy(&hand);

    // Two 10-value cards of different ranks can split
    Hand hand2;
    hand_init(&hand2);
    hand_add_card(&hand2, 10);  // Jack
    hand_add_card(&hand2, 11);  // Queen
    assert(hand_can_split(&hand2) == true);

    hand_destroy(&hand2);

    // Three cards cannot split
    Hand hand3;
    hand_init(&hand3);
    hand_add_card(&hand3, 5);
    hand_add_card(&hand3, 5);
    hand_add_card(&hand3, 5);
    assert(hand_can_split(&hand3) == false);

    hand_destroy(&hand3);
}

TEST(hand_double_detection) {
    Hand hand;
    hand_init(&hand);

    // Can double with 2 cards
    hand_add_card(&hand, 5);
    hand_add_card(&hand, 6);
    assert(hand_can_double(&hand) == true);

    // Cannot double with 3 cards
    hand_add_card(&hand, 2);
    assert(hand_can_double(&hand) == false);

    hand_destroy(&hand);
}

int main(void) {
    printf("Running Hand & Card Tests\n");
    printf("==================================\n\n");

    // Uncomment these as you implement the functions

    // Card utility tests
    run_test_card_values();
    run_test_card_ranks();
    run_test_card_suits();
    run_test_card_to_string();

    // Hand tests
    run_test_hand_initialization();
    run_test_hand_add_cards();
    run_test_hand_hard_values();
    run_test_hand_soft_values();
    run_test_hand_soft_becomes_hard();
    run_test_hand_multiple_aces();
    run_test_hand_bust_with_aces();
    run_test_hand_blackjack_detection();
    run_test_hand_split_detection();
    run_test_hand_double_detection();

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
