#ifndef _POKER_H
#define _POKER_H

#define WORDSIZE 64
#define BITS_WS 8


/* Define the Value and Suit enumerators */

struct card {
	enum {TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE} Value;
	enum {H, S, C, D} Suit;
};

struct player {
	struct card cards[5]; /* 5 cards for each player */
	unsigned int card_count[13]; /* counters for each of the 13 card values */
	unsigned long vector; /* 55 bit vector to manage strength of the cards */
};

/* Insert declarations for functions in poker.c here. */

struct card parse(const char *card);
void count_cards(struct player *p);
int is_flush(struct player *p);
int is_straight(struct player *p);
int is_straight_flush(struct player *p);
void eval_strength(struct player *p);
void compare_hands(FILE *fp);
int main(int argc, char *argv[]);

#endif
