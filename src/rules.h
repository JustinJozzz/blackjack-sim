#include <stdbool.h>

typedef struct {
    // Dealer rules
    bool dealer_hits_soft_17;
    bool dealer_peeks_blackjack;
    bool european_no_hole_card;
    
    // Player actions
    bool double_after_split;
    bool can_resplit_aces;
    bool can_hit_split_aces;
    bool late_surrender_allowed;
    bool early_surrender_allowed;
    
    // Doubling rules
    bool double_any_two_cards;  // false = restricted to 9-11 only
    
    // Splitting rules
    int max_splits;  // 3 = can split to 4 hands
    
    // Deck configuration
    int num_decks;
    double shoe_penetration;  // 0.75 = reshuffle at 75%
    
    // Payouts
    double blackjack_payout;  // 1.5 for 3:2, 1.2 for 6:5
    double insurance_payout;  // typically 2.0
    
    // Rare rules
    bool five_card_charlie;
    bool six_card_charlie;
} Rules;

void init_rules(Rules* rules);