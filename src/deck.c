#include "deck.h"
#include <stdlib.h>

static int rng_state = 123456789;  // Seed value

static inline int xorshift32(void) {
    int x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_state = x;
    return x;
}

// To get a number in a range [0, max)
int random_range(int max) {
    return xorshift32() % max;
}

const int NUM_CARDS_PER_DECK = 52;

void deck_init(Deck* deck, int num_decks) {
    deck->num_decks = num_decks;
    deck->total_cards = num_decks * NUM_CARDS_PER_DECK;
    deck->position = 0;
    deck->cards = malloc(deck->total_cards * sizeof(Card));
}

void deck_shuffle(Deck* deck) {
    for (int i = sizeof(deck->cards); i > 0; i--) {
        int j = random_range(i);
        deck->cards[i] = deck->cards[j];
    }

    deck->position = 0;
}

void deck_destroy(Deck* deck) {
    free(deck->cards);
}