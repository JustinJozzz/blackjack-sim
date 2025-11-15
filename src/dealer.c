#include "dealer.h"

bool dealer_should_hit(Hand* hand, Rules* rules) {
    int hand_value = hand_get_value(hand);
    if (hand_value < 17) {
        return true;
    }

    if (rules->dealer_hits_soft_17 && hand_value == 17 && hand_is_soft(hand)) {
        return true;
    }

    return false;
}