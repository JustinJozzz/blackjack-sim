#include "deck.h"
#include <stdlib.h>

#define NUM_CARDS_PER_DECK 52
static int rng_state = 123456789;  // Seed value

static inline int xorshift32(void) {
    int x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_state = x;
    return x;
}

// To get a number in a range [0, max) without modulo bias
int random_range(int max) {
    if (max <= 0) return 0;

    // Use rejection sampling to avoid modulo bias
    int limit = 0x7FFFFFFF - (0x7FFFFFFF % max);  // Largest multiple of max
    int value;

    do {
        value = xorshift32() & 0x7FFFFFFF;  // Force positive by masking sign bit
    } while (value >= limit);

    return value % max;
}

void deck_init(Deck* deck, int num_decks) {
    deck->num_decks = num_decks;
    deck->total_cards = num_decks * NUM_CARDS_PER_DECK;
    deck->position = 0;
    deck->cards = malloc(deck->total_cards * sizeof(int));
    
    for (int i = 0; i < deck->total_cards; i++) {
        deck->cards[i] = i % NUM_CARDS_PER_DECK;
    }
}

void deck_shuffle(Deck* deck) {
    for (int i = deck->total_cards - 1; i > 0; i--) {
        int j = random_range(i);
        int swap = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = swap;
    }

    deck->position = 0;
}

int deck_deal(Deck* deck) {
    int dealt_card = deck->cards[deck->position];
    deck->position++;
    return dealt_card;
}

void deck_destroy(Deck* deck) {
    free(deck->cards);
}

void deck_set_rng_seed(int seed) {
    rng_state = seed;
}