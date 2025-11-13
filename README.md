# Blackjack Strategy Simulator

A Monte Carlo simulation engine to calculate optimal blackjack basic strategy for different rule sets.

## Overview

This project simulates thousands of blackjack hands to determine the mathematically optimal play for any situation, given configurable game rules.

## Current Status

✅ **Phase 0: Foundation**
- Deck management (shuffle, deal, reshuffle)
- Custom random number generator (xorshift32)
- Test framework

🚧 **Next Steps: See IMPLEMENTATION_GUIDE.md**
- Card utilities
- Hand value calculation
- Game logic
- Basic strategy
- Simulation engine

## Project Structure

```
blackjack-sim/
├── src/
│   ├── deck.c/h          ✅ Deck operations
│   ├── card.c/h          ⏳ Card utilities (to implement)
│   ├── hand.c/h          ⏳ Hand management (to implement)
│   ├── rules.c/h         ⏳ Game rules (to implement)
│   ├── dealer.c/h        ⏳ Dealer logic (to implement)
│   ├── game.c/h          ⏳ Core game logic (to implement)
│   ├── strategy.c/h      ⏳ Basic strategy (to implement)
│   └── simulation.c/h    ⏳ Monte Carlo engine (to implement)
├── tests/
│   ├── test_game.c       ✅ Deck tests
│   ├── test_hand.c       ✅ Card & hand tests (ready to uncomment)
│   ├── test_game_logic.c ✅ Game logic tests (ready to uncomment)
│   └── test_strategy.c   ✅ Strategy tests (ready to uncomment)
├── ARCHITECTURE.md       📖 System design overview
├── IMPLEMENTATION_GUIDE.md 📖 Step-by-step implementation guide
└── Makefile              🔨 Build system
```

## Quick Start

### Run Current Tests
```bash
make build/test_game
./build/test_game
```

### Implement Phase 1: Card & Hand
See [IMPLEMENTATION_GUIDE.md](IMPLEMENTATION_GUIDE.md) for detailed instructions.

```bash
# 1. Create src/card.c and src/card.h
# 2. Create src/hand.c and src/hand.h
# 3. Uncomment tests in tests/test_hand.c
# 4. Build and test
make build/test_hand
./build/test_hand
```

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
- [ ] Card value calculations (A=11, face=10)
- [ ] Hand value with ace handling
- [ ] Soft/hard hand detection
- [ ] Blackjack detection
- [ ] Split and double detection
- [ ] Game rules configuration
- [ ] Dealer decision logic
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
