#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "../src/rules.h"

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
// Implement rules.h with these structures and functions:
//
// typedef struct {
//     bool dealer_hits_soft_17;
//     bool double_after_split;
//     bool late_surrender_allowed;
//     int max_splits;
//     double blackjack_payout;  // 1.5 for 3:2, 1.2 for 6:5
//     bool can_resplit_aces;
//     int num_decks;
// } Rules;
//
// void init_rules(Rules* rules) - Standard Vegas rules

TEST(standard_rules_initialization) {
    Rules rules;
    init_rules(&rules);
    
    // Standard 6-deck Vegas rules: S17, DAS, LSR
    assert(rules.dealer_hits_soft_17 == false);  // Dealer stands on soft 17
    assert(rules.double_after_split == true);
    assert(rules.late_surrender_allowed == true);
    assert(rules.max_splits == 3);               // Can split up to 4 hands
    assert(rules.blackjack_payout == 1.5);       // 3:2 payout
    assert(rules.can_resplit_aces == false);         // Cannot resplit aces
    assert(rules.num_decks == 6);
}

TEST(custom_rules_h17) {
    Rules rules;
    init_rules(&rules);
    
    // Modify to H17 (dealer hits soft 17 - worse for player)
    rules.dealer_hits_soft_17 = true;
    
    assert(rules.dealer_hits_soft_17 == true);
    assert(rules.double_after_split == true);  // Other rules unchanged
}

TEST(custom_rules_6_5_blackjack) {
    Rules rules;
    init_rules(&rules);
    
    // Modify to 6:5 blackjack (worse for player)
    rules.blackjack_payout = 1.2;  // 6:5 instead of 3:2
    
    assert(rules.blackjack_payout == 1.2);
    assert(rules.num_decks == 6);  // Other rules unchanged
}

TEST(custom_rules_single_deck) {
    Rules rules;
    init_rules(&rules);
    
    // Single deck game
    rules.num_decks = 1;
    
    assert(rules.num_decks == 1);
    assert(rules.blackjack_payout == 1.5);  // Still 3:2
}

TEST(custom_rules_no_surrender) {
    Rules rules;
    init_rules(&rules);
    
    // No surrender allowed
    rules.late_surrender_allowed = false;
    
    assert(rules.late_surrender_allowed == false);
    assert(rules.max_splits == 3);  // Other rules unchanged
}

TEST(custom_rules_no_das) {
    Rules rules;
    init_rules(&rules);
    
    // No double after split
    rules.double_after_split = false;
    
    assert(rules.double_after_split == false);
}

TEST(custom_rules_can_resplit_aces) {
    Rules rules;
    init_rules(&rules);
    
    // Allow resplitting aces (rare, favorable to player)
    rules.can_resplit_aces = true;
    
    assert(rules.can_resplit_aces == true);
}

TEST(custom_rules_max_splits) {
    Rules rules;
    init_rules(&rules);
    
    // Some casinos allow unlimited splits
    rules.max_splits = 10;  // Effectively unlimited
    
    assert(rules.max_splits == 10);
    
    // Some casinos allow no splits
    rules.max_splits = 0;
    assert(rules.max_splits == 0);
}

TEST(rules_european_no_hole_card) {
    // European rules: dealer doesn't take hole card until player done
    // This would require an additional flag in the future
    Rules rules;
    init_rules(&rules);
    
    // For now, just test we can create European-style rules
    rules.num_decks = 6;
    rules.dealer_hits_soft_17 = true;
    rules.late_surrender_allowed = false;
    
    assert(rules.num_decks == 6);
    assert(rules.dealer_hits_soft_17 == true);
    assert(rules.late_surrender_allowed == false);
}

TEST(rules_comparison) {
    Rules rules1, rules2;
    
    init_rules(&rules1);
    init_rules(&rules2);
    
    // Both should have identical settings
    assert(rules1.dealer_hits_soft_17 == rules2.dealer_hits_soft_17);
    assert(rules1.double_after_split == rules2.double_after_split);
    assert(rules1.late_surrender_allowed == rules2.late_surrender_allowed);
    assert(rules1.max_splits == rules2.max_splits);
    assert(rules1.blackjack_payout == rules2.blackjack_payout);
    assert(rules1.can_resplit_aces == rules2.can_resplit_aces);
    assert(rules1.num_decks == rules2.num_decks);
}

int main(void) {
    printf("Running Game Rules Tests\n");
    printf("==================================\n\n");

    // Uncomment these as you implement the functions

    run_test_standard_rules_initialization();
    run_test_custom_rules_h17();
    run_test_custom_rules_6_5_blackjack();
    run_test_custom_rules_single_deck();
    run_test_custom_rules_no_surrender();
    run_test_custom_rules_no_das();
    run_test_custom_rules_can_resplit_aces();
    run_test_custom_rules_max_splits();
    run_test_rules_european_no_hole_card();
    run_test_rules_comparison();

    printf("\n==================================\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_run);

    if (tests_passed == tests_run) {
        printf("All tests passed! \n");
        return 0;
    } else {
        printf("Some tests failed! \n");
        return 1;
    }
}
