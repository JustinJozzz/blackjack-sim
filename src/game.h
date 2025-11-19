#pragma once

#include <stdbool.h>
#include "deck.h"
#include "hand.h"
#include "rules.h"

typedef enum {
    HIT, STAND, DOUBLE, SPLIT, SURRENDER
} PlayerAction;

typedef struct {
    Deck deck;
    Hand* player_hands;
    int num_player_hands;
    double* player_bets;
    Hand dealer_hand;
    Rules rules;
    bool surrendered;
    bool game_over;
} GameState;

void game_init(GameState* game_state, Rules* rules, double initial_bet);

void game_deal_initial(GameState* game_state);

void game_play_action(GameState* game_state, PlayerAction player_action, int player_hand_index);

double game_resolve(GameState* game_state);

void game_destroy(GameState* game_state);