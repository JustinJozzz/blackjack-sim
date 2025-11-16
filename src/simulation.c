#include "simulation.h"
#include "game.h"
#include "card.h"

void simulation_run(SimulationConfig* simulation_config, SimulationResults* simulation_results) {
    for (int i = 0; i < simulation_config->num_hands; i++) {
        GameState game;
        game_init(&game, &simulation_config->rules, simulation_config->bet_per_hand);
        game_deal_initial(&game);

        PlayerAction curr_player_action = HIT;
        bool player_bust = false;
        while (curr_player_action != STAND && curr_player_action != SURRENDER && curr_player_action != DOUBLE && curr_player_action != SPLIT && !player_bust) {
            curr_player_action = get_basic_strategy_action(&game.player_hand, game.dealer_hand.cards[0], &simulation_config->rules, &simulation_config->strategy, true, true);
            game_play_action(&game, curr_player_action);

            int hand_value = hand_get_value(&game.player_hand);
            if (hand_value > 21) {
                player_bust = true;
            }
        }

        PlayerAction curr_dealer_action = HIT;
        bool dealer_bust = false;
        while (!player_bust && !dealer_bust && curr_dealer_action != STAND) {
            int hand_value = hand_get_value(&game.dealer_hand);
            if (hand_value >= 17) {
                curr_dealer_action = STAND;
            } else {
                curr_dealer_action = HIT;
            }

            if (curr_dealer_action == HIT) {
                hand_add_card(&game.dealer_hand, deck_deal(&game.deck));
            }
        }

        double multiplier = game_resolve(&game);
        if (multiplier == 0.0) {
            simulation_results->hands_lost++;
        }
        if (multiplier > 1.0) {
            simulation_results->hands_won++;
        }
        if (multiplier == 1.0) {
            simulation_results->hands_pushed++;
        }
        simulation_results->total_bet += game.bet;
        simulation_results->total_payout += multiplier * game.bet;
        simulation_results->hands_played++;
        simulation_results->house_edge = (simulation_results->total_bet - simulation_results->total_payout) / simulation_results->total_bet;
        game_destroy(&game);
    }
}