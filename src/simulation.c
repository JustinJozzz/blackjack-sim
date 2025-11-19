#include "simulation.h"
#include "game.h"
#include "card.h"

bool can_split(Rules *rules, int curr_player_hands, bool split_aces)
{
    if (curr_player_hands == rules->max_splits + 1)
    {
        return false;
    }

    if (!rules->can_resplit_aces && split_aces)
    {
        return false;
    }

    return true;
}

bool can_double(Rules *rules, PlayerAction curr_player_action, int cards_in_hand)
{
    if (curr_player_action == SPLIT && !rules->double_after_split)
    {
        return false;
    }

    if (cards_in_hand > 2)
    {
        return false;
    }

    return true;
}

void simulation_run(SimulationConfig *simulation_config, SimulationResults *simulation_results)
{
    for (int i = 0; i < simulation_config->num_hands; i++)
    {
        GameState game;
        game_init(&game, &simulation_config->rules, simulation_config->bet_per_hand);
        game_deal_initial(&game);

        int player_hand_index = 0;
        while (player_hand_index != game.num_player_hands)
        {
            PlayerAction curr_player_action = HIT;
            bool player_bust = false;
            bool split_aces = false;
            while ((curr_player_action == HIT || curr_player_action == SPLIT) && !player_bust && !split_aces)
            {
                curr_player_action = get_basic_strategy_action(&game.player_hands[player_hand_index],
                                                               game.dealer_hand.cards[0], &simulation_config->rules, &simulation_config->strategy,
                                                               can_split(&game.rules, game.num_player_hands, split_aces),
                                                               can_double(&game.rules, curr_player_action, game.player_hands[player_hand_index].num_cards));

                split_aces = false;
                // Track doubles and splits
                if (curr_player_action == DOUBLE)
                {
                    simulation_results->doubles_taken++;
                }
                else if (curr_player_action == SPLIT)
                {
                    if (card_rank(game.player_hands[player_hand_index].cards[0]) == 0 && card_rank(game.player_hands[player_hand_index].cards[1]) == 0)
                    {
                        split_aces = true;
                    }
                    simulation_results->splits_taken++;
                }

                game_play_action(&game, curr_player_action, player_hand_index);

                int hand_value = hand_get_value(&game.player_hands[player_hand_index]);
                if (hand_value > 21)
                {
                    player_bust = true;
                }
            }

            PlayerAction curr_dealer_action = HIT;
            bool dealer_bust = false;
            while (!player_bust && !dealer_bust && curr_dealer_action != STAND)
            {
                int hand_value = hand_get_value(&game.dealer_hand);
                if (hand_value >= 17)
                {
                    curr_dealer_action = STAND;
                }
                else
                {
                    curr_dealer_action = HIT;
                }

                if (curr_dealer_action == HIT)
                {
                    hand_add_card(&game.dealer_hand, deck_deal(&game.deck));
                }
            }

            player_hand_index++;
        }

        double round_payout = game_resolve(&game);
        double round_bets = 0.0;
        for (int i = 0; i < game.num_player_hands; i++) {
            round_bets += game.player_bets[i];
        }
        if (round_payout == 0)
        {
            simulation_results->hands_lost++;
        } else if (round_payout > round_bets)
        {
            simulation_results->hands_won++;
        }
        else if (round_payout == round_bets)
        {
            simulation_results->hands_pushed++;
        }
        simulation_results->total_bet += round_bets;
        simulation_results->total_payout += round_payout;
        simulation_results->hands_played++;
        simulation_results->house_edge = (simulation_results->total_bet - simulation_results->total_payout) / simulation_results->total_bet;
        game_destroy(&game);
    }
}