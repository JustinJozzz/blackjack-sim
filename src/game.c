#include "game.h"
#include <stdlib.h>

#define DEFUALT_HAND_SIZE 2

void game_init(GameState* game_state, Rules* rules, double initial_bet) {
    game_state->rules = *rules;
    Deck deck;
    deck_init(&deck, rules->num_decks);
    deck_shuffle(&deck);
    game_state->deck = deck;

    Hand player_hand;
    Hand dealer_hand;
    hand_init(&player_hand);
    hand_init(&dealer_hand);

    game_state->split_hands = malloc(sizeof(Hand) * rules->max_splits);
    game_state->num_split_hands = 0;

    game_state->bet = initial_bet;
    game_state->game_over = false;
}

void game_deal_initial(GameState* game_state) {
    for (int i = 0; i < DEFUALT_HAND_SIZE; i++) {
        hand_add_card(&game_state->player_hand, deck_deal(&game_state->deck));
    }

    for (int i = 0; i < DEFUALT_HAND_SIZE; i++) {
        hand_add_card(&game_state->player_hand, deck_deal(&game_state->deck));
    }
}

void game_destroy(GameState* game_state) {
    free(game_state->split_hands);
}