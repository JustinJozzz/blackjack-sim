#include "game.h"
#include <stdlib.h>

#define DEFUALT_HAND_SIZE 2
#define DEFAULT_MULTIPLIER 1.0
#define REGULAR_WIN_MULTIPLIER 1.0

void game_init(GameState* game_state, Rules* rules, double initial_bet) {
    game_state->rules = *rules;
    deck_init(&game_state->deck, rules->num_decks);
    deck_shuffle(&game_state->deck);

    hand_init(&game_state->player_hand);
    hand_init(&game_state->dealer_hand);

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
        hand_add_card(&game_state->dealer_hand, deck_deal(&game_state->deck));
    }
}

void game_play_action(GameState* game_state, PlayerAction player_action) {
    switch (player_action) {
        case HIT:
            hand_add_card(&game_state->player_hand, deck_deal(&game_state->deck));
            break;
        case STAND:
            break;
        default:
            break;
    }
}

double game_resolve(GameState* game_state) {
    int player_hand_value = hand_get_value(&game_state->player_hand);
    int dealer_hand_value = hand_get_value(&game_state->dealer_hand);
    game_state->game_over = true;

    if (dealer_hand_value > 21) {
        return DEFAULT_MULTIPLIER + REGULAR_WIN_MULTIPLIER;
    }

    if (player_hand_value > 21 || player_hand_value < dealer_hand_value) {
        return 0.0;
    }

    if (hand_is_blackjack(&game_state->player_hand)) {
        return DEFAULT_MULTIPLIER + game_state->rules.blackjack_payout;
    }

    if (player_hand_value > dealer_hand_value) {
        return DEFAULT_MULTIPLIER + REGULAR_WIN_MULTIPLIER;
    }

    if (player_hand_value == dealer_hand_value) {
        return DEFAULT_MULTIPLIER;
    }

    return DEFAULT_MULTIPLIER;
}

void game_destroy(GameState* game_state) {
    hand_destroy(&game_state->player_hand);
    hand_destroy(&game_state->dealer_hand);
    deck_destroy(&game_state->deck);
    free(game_state->split_hands);
}