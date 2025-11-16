#include "hand.h"
#include "rules.h"
#include "game.h"

typedef enum {
    YES,
    NO,
    YES_IF_SPLIT_OFFERED
} SplitAction;

typedef enum {
    SOFT_HIT,
    SOFT_STAND,
    SOFT_DOUBLE_OR_HIT,
    SOFT_DOUBLE_OR_STAND
} SoftHandPlayerAction;

typedef struct {
    PlayerAction hard_totals[10][9];
    SoftHandPlayerAction soft_totals[10][8];
    SplitAction pairs[10][10];    
    bool surrender[10][3];
} BasicStrategy;

void basic_strategy_init(BasicStrategy* basic_strategy);

PlayerAction get_basic_strategy_action(Hand* player_hand, int dealer_up_card, Rules* rules, BasicStrategy* strategy, bool can_split, bool can_double);