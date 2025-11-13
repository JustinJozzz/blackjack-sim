#include "card.h"
#include "stdio.h"

#define NUM_CARDS_PER_DECK 52
#define NUM_VALUES 13

static const char* card_values[] = {
    "Ace", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King"
};

static const char* card_suits[] = {
    "Diamonds", "Spades", "Hearts", "Clubs"
};

int card_value (int card) {
    int card_value = card % NUM_VALUES;
    
    if (card_value == 0) {
        return 11;
    }
    
    if (card_value > 9) {
        return 10;
    }

    return card_value + 1;
}

int card_rank(int card) {
    return card % NUM_VALUES;
}

int card_suit(int card) {
    return card / NUM_VALUES;
}

char* card_to_string(int card) {
    static char buffer[32];
    snprintf(buffer, sizeof(buffer), "%s of %s", card_values[card_rank(card)], card_suits[card_suit(card)]);
    return buffer;
}