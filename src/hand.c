#include "hand.h"
#include "card.h"
#include <stdlib.h>

void hand_init(Hand* hand) {
    hand->num_cards = 0;
    hand->capacity = 2;
    hand->cards = malloc(sizeof(int) * hand->capacity);
}

void hand_add_card(Hand* hand, int card) {
    if (hand->num_cards == hand->capacity) {
        hand->cards = realloc(hand->cards, hand->capacity + 1);
    }

    hand->cards[hand->num_cards] = card;
    hand->num_cards++;
}

int compare_cards(const void* a, const void* b) {
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;

    int card1_val = card_value(arg1);
    int card2_val = card_value(arg2);
    
    if (card1_val < card2_val) return -1;
    if (card1_val > card2_val) return 1;
    return 0;
}

int hand_get_value(Hand* hand) {
    int sum = 0;
    qsort(hand->cards, hand->num_cards, sizeof(int), compare_cards);

    for (int i = 0; i < hand->num_cards; i++) {
        int val = card_value(hand->cards[i]);
        if (sum + val > 21 && val == 11) {
            sum += 1;
        } else {
            sum += card_value(hand->cards[i]);
        }
    }

    return sum;
}

bool hand_is_soft(Hand* hand) {
    int sum = 0;
    qsort(hand->cards, hand->num_cards, sizeof(int), compare_cards);

    for (int i = 0; i < hand->num_cards; i++) {
        int val = card_value(hand->cards[i]);
        if (sum + val > 21 && val == 11) {
            sum += 1;
        } else if (val == 11) {
            return true;
        } else {
            sum += card_value(hand->cards[i]);
        }
    }

    return false;
}

void hand_destroy(Hand* hand) {
    free(hand->cards);
}