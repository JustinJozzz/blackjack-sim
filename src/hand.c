#include "hand.h"
#include "card.h"
#include <stdlib.h>

#define ACE_MAX_VAL 11
#define ACE_MIN_VAL 1
#define BLACKJACK_VAL 21

void hand_init(Hand* hand) {
    hand->num_cards = 0;
    hand->capacity = 2;
    hand->cards = malloc(sizeof(int) * hand->capacity);
}

void hand_add_card(Hand* hand, int card) {
    if (hand->num_cards == hand->capacity) {
        hand->capacity++;
        hand->cards = realloc(hand->cards, sizeof(int) * hand->capacity);
    }

    hand->cards[hand->num_cards] = card;
    hand->num_cards++;
}

int hand_pop_card(Hand* hand) {
    int pop_card = hand->cards[hand->num_cards-1];
    hand->num_cards--;

    return pop_card;
}

int hand_get_value(Hand* hand) {
    int sum = 0;
    int ace_count = 0;

    for (int i = 0; i < hand->num_cards; i++) {
        int val = card_value(hand->cards[i]);
        if (val == ACE_MAX_VAL) {
            ace_count++;
        } else {
            sum += card_value(hand->cards[i]);
        }
    }

    for (int i = 0; i < ace_count; i++) {
        if (sum + ACE_MAX_VAL <= BLACKJACK_VAL) {
            sum += ACE_MAX_VAL;
        } else {
            sum += 1;
        }
    }

    return sum;
}

bool hand_is_soft(Hand* hand) {
    int sum = 0;
    bool has_ace = false;

    for (int i = 0; i < hand->num_cards; i++) {
        int val = card_value(hand->cards[i]);
        if (val != ACE_MAX_VAL) 
        {
            sum += val;
        }

        if (card_rank(hand->cards[i]) == 0) 
        {
            has_ace = true;
        }
    }

    if (has_ace && sum <= BLACKJACK_VAL - ACE_MAX_VAL) {
        return true;
    }

    return false;
}

bool hand_is_blackjack(Hand* hand) {
    if (hand->num_cards > 2) {
        return false;
    }

    return hand_get_value(hand) == BLACKJACK_VAL;
}

bool hand_can_split(Hand* hand) {
    if (hand->num_cards != 2) {
        return false;
    }

    if (card_value(hand->cards[0]) != card_value(hand->cards[1])) {
        return false;
    }

    return true;
}

bool hand_can_double(Hand* hand) {
    return hand->num_cards == 2;
}

void hand_destroy(Hand* hand) {
    free(hand->cards);
}