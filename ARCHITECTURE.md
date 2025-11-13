# Blackjack Simulator Architecture

## Overview
This simulator will calculate optimal basic strategy by running Monte Carlo simulations with different game parameters.

## Recommended Structure

### 1. Core Components (in order of implementation)

#### Phase 1: Card & Hand Management
- **card.h/card.c**: Card utility functions
  - `int card_value(int card)` - Get blackjack value (A=11, face=10, etc.)
  - `int card_rank(int card)` - Get rank 0-12 (A-K)
  - `char* card_to_string(int card)` - For debugging/display

- **hand.h/hand.c**: Hand data structure and operations
  - `Hand` struct to track cards in a hand
  - `hand_init()`, `hand_add_card()`, `hand_destroy()`
  - `hand_get_value()` - Calculate hand value with ace handling
  - `hand_is_soft()` - Check if hand contains usable ace
  - `hand_is_blackjack()` - Check for natural blackjack
  - `hand_can_split()` - Check if hand is splittable
  - `hand_can_double()` - Check if hand can double down

#### Phase 2: Game Rules & State
- **rules.h/rules.c**: Game parameters
  - `GameRules` struct for configurable rules:
    - Dealer hits/stands on soft 17
    - Double after split allowed
    - Surrender allowed (early/late)
    - Number of splits allowed
    - Blackjack payout (3:2 or 6:5)
    - Resplit aces allowed
    - Number of decks

- **game_state.h/game_state.c**: Game state management
  - `GameState` struct to track current game:
    - Dealer hand
    - Player hand(s) (for splits)
    - Current bet
    - Deck
    - Rules
  - Initialize, update, and query game state

#### Phase 3: Game Logic
- **actions.h**: Player action enum
  - `PlayerAction` enum: HIT, STAND, DOUBLE, SPLIT, SURRENDER

- **dealer.h/dealer.c**: Dealer logic
  - `dealer_should_hit()` - Determine if dealer hits based on rules
  - `dealer_play()` - Play out dealer's hand according to rules

- **game.h/game.c**: Core game logic
  - `game_init()` - Set up a new game
  - `game_deal_initial()` - Deal initial cards
  - `game_play_action()` - Execute a player action
  - `game_resolve()` - Determine winner and payout
  - `game_destroy()` - Clean up

#### Phase 4: Strategy & Simulation
- **strategy.h/strategy.c**: Basic strategy logic
  - `PlayerAction get_basic_strategy_action(Hand* player, int dealer_upcard, GameRules* rules)`
  - Strategy tables/logic based on mathematical optimal play

- **simulation.h/simulation.c**: Monte Carlo simulation
  - `SimulationConfig` - Parameters for simulation runs
  - `SimulationResults` - Track outcomes, EV for each decision
  - `run_simulation()` - Run N hands and collect statistics
  - `calculate_expected_values()` - Compute EV for each hand situation

## Data Flow

1. **Setup**: Create deck(s), initialize game rules
2. **Deal**: Create game state, deal initial cards to player/dealer
3. **Player Decision**: Query basic strategy for action based on:
   - Player hand value and composition
   - Dealer upcard
   - Game rules
4. **Player Action**: Execute action (hit/stand/double/split/surrender)
5. **Dealer Play**: Dealer plays according to rules
6. **Resolution**: Compare hands, calculate payout
7. **Statistics**: Record outcome for strategy calculation

## Simulation Strategy

To calculate basic strategy:
1. For each possible player hand total (hard 4-20, soft 12-21, pairs)
2. For each dealer upcard (A-10)
3. For each legal action in that situation
4. Run N simulations and track average outcome
5. Optimal action = highest expected value

## Testing Strategy

Build incrementally with tests for each component:
1. Test card utilities
2. Test hand value calculation (especially ace handling)
3. Test dealer logic with various rules
4. Test individual game actions
5. Test full game flow
6. Test strategy decisions
7. Validate simulation results against known basic strategy tables
