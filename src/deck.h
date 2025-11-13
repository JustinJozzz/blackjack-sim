typedef struct {
    int* cards;
    int num_decks;
    int total_cards;
    int position;
} Deck;

void deck_init(Deck* deck, int num_decks);

void deck_shuffle(Deck* deck);

int deck_deal(Deck* deck);

void deck_destroy(Deck* deck);