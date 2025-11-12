typedef struct {
    int suit;
    int value;
} Card;

typedef struct {
    Card* cards;
    int num_decks;
    int total_cards;
    int position;
} Deck;

void deck_init(Deck* deck, int num_decks);

void deck_shuffle(Deck* deck);

void deck_destroy(Deck* deck);