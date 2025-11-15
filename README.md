# Blackjack Strategy Simulator

A Monte Carlo simulation engine to calculate optimal blackjack basic strategy for different rule sets.

## Overview

This project simulates thousands of blackjack hands to determine the mathematically optimal play for any situation, given configurable game rules.

## Current Status

✅ **Phase 0: Foundation**
- Deck management (shuffle, deal, reshuffle)
- Custom random number generator (xorshift32)
- Test framework

✅ **Phase 1: Card & Hand Management** (14/14 tests passing)
- Card value calculations (A=11, face=10)
- Card rank and suit utilities
- Hand initialization and dynamic array management
- Hand value calculation with ace handling
- Soft/hard hand detection
- Blackjack detection
- Split detection
- Double detection

✅ **Phase 2: Game Rules** (10/10 tests passing)
- Standard Vegas rules (S17, DAS, LSR, 3:2, 6-deck)
- Configurable rule variations (H17, 6:5, single deck, etc.)

🚧 **Phase 2: Dealer Logic** (4/4 tests passing)
- Dealer hit/stand decision logic
- Soft 17 handling

⏳ **Next Steps: See IMPLEMENTATION_GUIDE.md**
- Game state management
- Player actions (hit, stand, double, split, surrender)
- Payout calculations
- Basic strategy
- Simulation engine

## Project Structure

```
blackjack-sim/
├── src/
│   ├── deck.c/h          ✅ Deck operations
│   ├── card.c/h          ✅ Card utilities (4/4 tests passing)
│   ├── hand.c/h          ✅ Hand management (14/14 tests passing)
│   ├── rules.c/h         ✅ Game rules (10/10 tests passing)
│   ├── dealer.c/h        🚧 Dealer logic (4/4 tests passing)
│   ├── game.c/h          ⏳ Core game logic (to implement)
│   ├── strategy.c/h      ⏳ Basic strategy (to implement)
│   └── simulation.c/h    ⏳ Monte Carlo engine (to implement)
├── tests/
│   ├── test_game.c       ✅ Deck tests (3/3 passing)
│   ├── test_hand.c       ✅ Card & hand tests (14/14 passing)
│   ├── test_rules.c      ✅ Rules tests (10/10 passing)
│   ├── test_game_logic.c 🚧 Dealer & game tests (4 passing, ready to uncomment more)
│   └── test_strategy.c   ✅ Strategy tests (ready to uncomment)
├── .vscode/              🔧 VS Code debug configurations
├── ARCHITECTURE.md       📖 System design overview
├── IMPLEMENTATION_GUIDE.md 📖 Step-by-step implementation guide
└── Makefile              🔨 Build system
```

## Quick Start

### Run All Tests
```bash
make test
```

### Current Test Results
- ✅ Deck operations: 3/3 tests passing
- ✅ Card utilities: 4/4 tests passing
- ✅ Hand management: 14/14 tests passing
- ✅ Game rules: 10/10 tests passing
- ✅ Dealer logic: 4/4 tests passing

**Total: 35/35 tests passing**

### Next Steps
Implement game state management in [src/game.c](src/game.c) and [src/game.h](src/game.h):
- Game initialization and cleanup
- Initial deal (2 cards to player, 2 to dealer)
- Player actions (hit, stand, double, split, surrender)
- Game resolution and payout calculation

See [tests/test_game_logic.c](tests/test_game_logic.c#L143-L332) for commented-out tests ready to uncomment.

### Build Everything
```bash
make          # Build main executable
make clean    # Clean build artifacts
```

## Implementation Approach

This project follows **Test-Driven Development (TDD)**:

1. ✅ Tests are already written for you
2. 📝 Uncomment tests one at a time
3. ❌ Watch them fail
4. 💻 Implement the feature
5. ✅ Watch tests pass
6. 🔁 Repeat

## Features to Implement

### Core Functionality
- [x] Card value calculations (A=11, face=10)
- [x] Card rank and suit utilities
- [x] Hand initialization and dynamic arrays
- [x] Hand value with ace handling
- [x] Soft/hard hand detection
- [x] Blackjack detection
- [x] Split and double detection
- [x] Game rules configuration
- [x] Dealer decision logic (hit/stand)
- [ ] Game state management
- [ ] Player action handling
- [ ] Payout calculation
- [ ] Full game simulation
- [ ] Basic strategy lookup
- [ ] Monte Carlo simulation engine

### Advanced Features
- [ ] Expected value calculations
- [ ] Strategy table generation from simulation
- [ ] Split hand support
- [ ] Surrender support
- [ ] Multiple rule set comparison
- [ ] Card counting integration
- [ ] Performance optimization

## Expected Results

With perfect basic strategy and standard rules (6-deck, S17, DAS):
- **House Edge**: ~0.40-0.50%
- **Win Rate**: ~42-43%
- **Loss Rate**: ~48-49%
- **Push Rate**: ~8-9%
- **Blackjack Frequency**: ~4.8%

## Documentation

- **[ARCHITECTURE.md](ARCHITECTURE.md)**: High-level system design and data flow
- **[IMPLEMENTATION_GUIDE.md](IMPLEMENTATION_GUIDE.md)**: Detailed step-by-step implementation instructions
- **Test files**: Each test file contains detailed comments explaining expected behavior

## Game Rules Supported

The simulator will support various rule configurations:
- Number of decks (1, 2, 4, 6, 8)
- Dealer hits/stands on soft 17
- Double after split
- Resplit aces
- Surrender (early/late)
- Blackjack payout (3:2 or 6:5)
- Maximum number of splits

## Building

```bash
# Standard build
make

# Debug build with symbols
make debug

# Optimized release build
make release

# Run tests
make build/test_game
make build/test_hand
make build/test_game_logic
make build/test_strategy

# Clean
make clean
```

## Contributing

This is a personal learning project. Implementation is intentionally left to the developer to learn C programming, algorithm design, and probability theory.

## Resources

- [Wizard of Odds - Blackjack Calculator](https://wizardofodds.com/games/blackjack/calculator/)
- [Blackjack Basic Strategy](https://wizardofodds.com/games/blackjack/strategy/calculator/)
- "Beat the Dealer" by Edward Thorp
- "The Theory of Blackjack" by Peter Griffin

## License

MIT License - Feel free to use for learning purposes.
