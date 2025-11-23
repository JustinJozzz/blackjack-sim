#include "rules.h"


void rules_init(Rules* rules) {
    // Dealer rules
    rules->dealer_hits_soft_17 = false;        // S17 (dealer stands)
    rules->dealer_peeks_blackjack = true;      // US rules
    rules->european_no_hole_card = false;      // US rules
    
    // Player actions
    rules->double_after_split = true;          // DAS allowed
    rules->can_resplit_aces = false;           // Usually not allowed
    rules->can_hit_split_aces = false;         // Usually only one card
    rules->late_surrender_allowed = true;      // Standard in many casinos
    rules->early_surrender_allowed = false;    // Rare
    
    // Doubling rules
    rules->double_any_two_cards = true;        // Most liberal
    
    // Splitting rules
    rules->max_splits = 3;                     // Can split to 4 hands
    
    // Deck configuration
    rules->num_decks = 6;                      // Most common
    rules->shoe_penetration = 0.75;            // 75% penetration
    
    // Payouts
    rules->blackjack_payout = 1.5;             // 3:2 (traditional)
    rules->insurance_payout = 2.0;             // Standard 2:1
    
    // Rare rules
    rules->five_card_charlie = false;          // Not standard
    rules->six_card_charlie = false;           // Not standard
}