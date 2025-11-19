#include "rules.h"
#include "strategy.h"

typedef struct {
    int num_hands;
    Rules rules;
    BasicStrategy strategy;
    double bet_per_hand;
} SimulationConfig;

typedef struct {
    int hands_played;
    int hands_won;
    int hands_lost;
    int hands_pushed;
    int doubles_taken;
    int splits_taken;
    double total_bet;
    double total_payout;
    double house_edge;
} SimulationResults;

void simulation_run(SimulationConfig* simulation_config_init, SimulationResults* simulation_results);