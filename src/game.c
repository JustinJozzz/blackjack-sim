#include "game.h"
#include <stdlib.h>

#define DEFUALT_HAND_SIZE 2
#define DEFAULT_MULTIPLIER 1.0
#define SURRENDER_MULTIPLIER 0.5
#define REGULAR_WIN_MULTIPLIER 1.0

void game_init(GameState* game_state, Rules* rules, double initial_bet) {
    game_state->rules = *rules;
    deck_init(&game_state->deck, rules->num_decks);
    deck_shuffle(&game_state->deck);

    game_state->player_hands = malloc(sizeof(Hand) * rules->max_splits + 1);

    hand_init(&game_state->player_hands[0]);
    game_state->num_player_hands = 0;
    hand_init(&game_state->dealer_hand);

    game_state->player_bets = malloc(sizeof(double) * rules->max_splits + 1);
    game_state->player_bets[0] = initial_bet;
    game_state->surrendered = false;
    game_state->game_over = false;
}

void game_deal_initial(GameState* game_state) {
    for (int i = 0; i < DEFUALT_HAND_SIZE; i++) {
        hand_add_card(&game_state->player_hands[game_state->num_player_hands], deck_deal(&game_state->deck));
    }
    game_state->num_player_hands++;

    for (int i = 0; i < DEFUALT_HAND_SIZE; i++) {
        hand_add_card(&game_state->dealer_hand, deck_deal(&game_state->deck));
    }
}

void game_play_action(GameState* game_state, PlayerAction player_action, int player_hand_index) {
    switch (player_action) {
        case HIT:
            hand_add_card(&game_state->player_hands[player_hand_index], deck_deal(&game_state->deck));
            break;
        case STAND:
            break;
        case SPLIT:
            int pop_card = hand_pop_card(&game_state->player_hands[player_hand_index]);
            hand_add_card(&game_state->player_hands[game_state->num_player_hands], pop_card);
            game_state->player_bets[game_state->num_player_hands] = game_state->player_bets[player_hand_index];
            hand_add_card(&game_state->player_hands[player_hand_index], deck_deal(&game_state->deck));
            hand_add_card(&game_state->player_hands[game_state->num_player_hands], deck_deal(&game_state->deck));
            game_state->num_player_hands++;
            break;
        case DOUBLE:
            game_state->player_bets[player_hand_index] *= 2;
            hand_add_card(&game_state->player_hands[player_hand_index], deck_deal(&game_state->deck));
            break;
        case SURRENDER:
            game_state->surrendered = true;
            break;
        default:
            break;
    }
}

double game_resolve(GameState* game_state) {
    double total_payout = 0;

    for (int i = 0; i < game_state->num_player_hands; i++) {
        double multiplier = DEFAULT_MULTIPLIER;
        int player_hand_value = hand_get_value(&game_state->player_hands[i]);
        int dealer_hand_value = hand_get_value(&game_state->dealer_hand);
        game_state->game_over = true;

        
        if (game_state->surrendered) {
            multiplier = SURRENDER_MULTIPLIER;
        } else if (dealer_hand_value > 21) {
            multiplier += REGULAR_WIN_MULTIPLIER;
        } else if (player_hand_value > 21 || player_hand_value < dealer_hand_value) {
            multiplier = 0.0;
        } else if (hand_is_blackjack(&game_state->player_hands[i]) && game_state->num_player_hands == 1) {
            multiplier += game_state->rules.blackjack_payout;
        } else if (player_hand_value > dealer_hand_value) {
            multiplier += REGULAR_WIN_MULTIPLIER;
        }

        total_payout += game_state->player_bets[i] * multiplier;
    }

    return total_payout;
}

void game_destroy(GameState* game_state) {
    free(game_state->player_hands);
    free(game_state->player_bets);
    hand_destroy(&game_state->dealer_hand);
    deck_destroy(&game_state->deck);
}