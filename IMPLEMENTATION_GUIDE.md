# Blackjack Simulator Implementation Guide

This guide outlines the recommended implementation order with tests already written for each phase.

## Quick Start

All test files are already created in the `tests/` directory. Simply uncomment tests as you implement each feature.

## Phase 1: Card & Hand Management

### Files to Create:
- `src/card.h` and `src/card.c`
- `src/hand.h` and `src/hand.c`

### Test File:
- `tests/test_hand.c` (already created)

### Implementation Order:

#### 1. Card Utilities (`card.h/card.c`)
Implement these functions:
```c
int card_value(int card);        // Returns blackjack value (A=11, 2-10, face=10)
int card_rank(int card);         // Returns rank 0-12 (0=A, 1-9=2-10, 10=J, 11=Q, 12=K)
const char* card_to_string(int card);  // For debugging
```

**Why this order?** Hands depend on card values, so implement cards first.

#### 2. Hand Structure (`hand.h/hand.c`)
```c
typedef struct {
    int* cards;
    int num_cards;
    int capacity;
} Hand;

void hand_init(Hand* hand);
void hand_destroy(Hand* hand);
void hand_add_card(Hand* hand, int card);
int hand_get_value(Hand* hand);      // Handle aces as 1 or 11
bool hand_is_soft(Hand* hand);       // Has usable ace counted as 11
bool hand_is_blackjack(Hand* hand);  // Natural 21
bool hand_can_split(Hand* hand);     // Two cards same rank
bool hand_can_double(Hand* hand);    // Exactly 2 cards
```

**Key Challenge:** `hand_get_value()` must correctly handle aces. Algorithm:
1. Count aces and sum all card values (aces as 11)
2. While sum > 21 and we have aces counted as 11, convert one ace to 1
3. Return best value ≤ 21, or bust value if over

**To Test:**
```bash
# Uncomment tests in test_hand.c, then:
make build/test_hand
./build/test_hand
```

## Phase 2: Game Rules & State

### Files to Create:
- `src/rules.h` and `src/rules.c`
- `src/dealer.h` and `src/dealer.c`
- `src/game.h` and `src/game.c`

### Test File:
- `tests/test_game_logic.c` (already created)

### Implementation Order:

#### 3. Game Rules (`rules.h/rules.c`)
```c
typedef struct {
    bool dealer_hits_soft_17;
    bool double_after_split;
    bool surrender_allowed;
    int max_splits;
    double blackjack_payout;  // 1.5 for 3:2, 1.2 for 6:5
    bool resplit_aces;
    int num_decks;
} GameRules;

void rules_init_standard(GameRules* rules);
```

**Why this order?** Rules are needed by dealer logic and game state.

#### 4. Dealer Logic (`dealer.h/dealer.c`)
```c
bool dealer_should_hit(Hand* dealer_hand, GameRules* rules);
void dealer_play(Hand* dealer_hand, Deck* deck, GameRules* rules);
```

**Logic:**
- Hit on 16 or less
- Hit on soft 17 if `dealer_hits_soft_17` is true
- Stand on hard 17+
- Stand on soft 18+

#### 5. Game State & Actions (`game.h/game.c`)
```c
typedef enum {
    HIT,
    STAND,
    DOUBLE,
    SPLIT,
    SURRENDER
} PlayerAction;

typedef struct {
    Deck deck;
    Hand dealer_hand;
    Hand player_hand;
    GameRules rules;
    double bet;
    bool game_over;
} GameState;

void game_init(GameState* game, GameRules* rules, double initial_bet);
void game_destroy(GameState* game);
void game_deal_initial(GameState* game);
bool game_play_action(GameState* game, PlayerAction action);
double game_resolve(GameState* game);  // Returns payout multiplier
```

**Payout multipliers:**
- Player bust: 0.0
- Dealer bust (player didn't): 2.0 (bet + winnings)
- Player blackjack (dealer didn't): 2.5 (bet + 1.5x winnings for 3:2)
- Player wins: 2.0
- Push: 1.0
- Player loses: 0.0

**To Test:**
```bash
make build/test_game_logic
./build/test_game_logic
```

## Phase 3: Basic Strategy & Simulation

### Files to Create:
- `src/strategy.h` and `src/strategy.c`
- `src/simulation.h` and `src/simulation.c`

### Test File:
- `tests/test_strategy.c` (already created)

### Implementation Order:

#### 6. Basic Strategy (`strategy.h/strategy.c`)
```c
PlayerAction get_basic_strategy_action(
    Hand* player,
    int dealer_upcard,
    GameRules* rules,
    bool can_split,
    bool can_double
);
```

**Implementation Approaches:**

**Option A: Lookup Tables (Recommended)**
- Create arrays for hard totals, soft totals, and pairs
- Index by player total and dealer upcard
- Fastest, most accurate

**Option B: Rule-Based**
- Series of if/else statements
- Easier to understand and modify
- Still fast enough for simulation

**Basic Strategy Quick Reference:**
- Always split 8s and Aces
- Never split 10s, 4s, 5s
- Always double on 11 (vs any)
- Always hit hard 11 or less
- Always stand on hard 17+
- Stand on hard 12-16 if dealer shows 2-6 (dealer bust cards)
- Soft hands are more aggressive (hit soft 17, double soft 13-18 vs 5-6)

#### 7. Simulation Engine (`simulation.h/simulation.c`)
```c
typedef struct {
    int num_hands;
    GameRules rules;
    bool use_basic_strategy;
} SimulationConfig;

typedef struct {
    int hands_played;
    int hands_won;
    int hands_lost;
    int hands_pushed;
    int blackjacks;
    double total_bet;
    double total_payout;
    double house_edge;
} SimulationResults;

void simulation_run(SimulationConfig* config, SimulationResults* results);
double simulation_get_ev(SimulationResults* results);
```

**Algorithm:**
1. Initialize deck and shuffle
2. For each hand:
   - Deal initial cards
   - If player has blackjack, resolve immediately
   - Otherwise, play using strategy
   - Resolve and record outcome
3. Calculate statistics

**Expected Results:**
- With perfect basic strategy and standard rules: house edge ~0.5%
- Win rate: ~42-43%
- Loss rate: ~48-49%
- Push rate: ~8-9%
- Blackjack frequency: ~4.8%

**To Test:**
```bash
make build/test_strategy
./build/test_strategy
```

## Phase 4: Advanced - Calculate Optimal Strategy

### Goal
Instead of hard-coding basic strategy, calculate it from scratch using simulation.

### Approach:
For each situation (player total, dealer upcard):
1. Run N simulations for each legal action
2. Calculate expected value for each action
3. Optimal action = highest EV

### Example:
```c
double calculate_situation_ev(
    int player_total,
    bool is_soft,
    bool is_pair,
    int dealer_upcard,
    PlayerAction action,
    GameRules* rules,
    int num_simulations
);
```

Run this for all combinations:
- Player totals: Hard 4-21, Soft 12-21, Pairs 2-2 through A-A
- Dealer upcards: A, 2-10
- Actions: HIT, STAND, DOUBLE, SPLIT (where legal)

Generate a strategy table showing optimal action for each situation.

## Testing Strategy

### Test-Driven Development Workflow:
1. Uncomment a test in the test file
2. Compile: `make build/test_<name>`
3. Run: `./build/test_<name>`
4. See it fail
5. Implement the feature
6. Recompile and run until it passes
7. Move to next test

### Makefile Commands:
```bash
make                    # Build main executable
make build/test_hand    # Build hand tests
make build/test_game_logic    # Build game logic tests
make build/test_strategy      # Build strategy tests
make clean              # Clean build artifacts
```

## Common Pitfalls

1. **Ace Handling**: Most complex part. Test thoroughly with multiple aces.
2. **Dealer Logic**: Remember soft 17 rule variations.
3. **Split Hands**: Initial implementation can skip this - add later.
4. **Double Down**: Can only double on initial 2 cards.
5. **Blackjack Detection**: Only natural 21 (2 cards) counts as blackjack.
6. **Shuffle Point**: Reshuffle when deck gets low (e.g., < 20% remaining).

## Validation

Compare your simulation results to known values:
- [Wizard of Odds](https://wizardofodds.com/games/blackjack/calculator/)
- Standard 6-deck S17 DAS: House edge ≈ 0.40%
- Standard 6-deck H17 DAS: House edge ≈ 0.55%

## Next Steps After Basic Implementation

1. **Add Split Handling**: Track multiple hands, play each independently
2. **Surrender**: Early vs late surrender logic
3. **Card Counting**: Track running/true count, vary bets
4. **Strategy Deviations**: Modify basic strategy based on count
5. **Visualization**: Output strategy tables, graphs of results
6. **Optimization**: Profile and optimize hot paths
7. **Rule Variations**: European no-hole-card, dealer peek, etc.

## Resources

- **Basic Strategy**: [Wizard of Odds Basic Strategy](https://wizardofodds.com/games/blackjack/strategy/calculator/)
- **House Edge Calculator**: [Blackjack Review](http://www.blackjackreview.com/wp/encyclopedia/house-edge-calculator/)
- **Card Counting**: "Beat the Dealer" by Edward Thorp
- **Mathematics**: "The Theory of Blackjack" by Peter Griffin

## Questions?

Each test file has detailed comments explaining:
- What structures to create
- What functions to implement
- Expected behavior
- Edge cases to handle

Start with Phase 1, uncomment tests one at a time, and implement incrementally. Good luck!
