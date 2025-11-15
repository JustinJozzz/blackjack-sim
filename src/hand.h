#include <stdbool.h>

typedef struct {
    int* cards;
    int num_cards;
    int capacity;
} Hand;

void hand_init(Hand* hand);

void hand_add_card(Hand* hand, int card);

int hand_get_value(Hand* hand);

bool hand_is_soft(Hand* hand);

bool hand_is_blackjack(Hand* hand);

bool hand_can_split(Hand* hand);

bool hand_can_double(Hand* hand);

void hand_destroy(Hand* hand);