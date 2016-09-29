#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "poker.h"

/* Global instances of the two players */
struct player P1, P2;

/* Parse input and store in card data structure. */
/* 5H 5C 6S 7S KD  or 5H single */
/* Parse card from a 2 char input */
struct card parse(const char *card)
{
	struct card c;
	/* Pull Value */
	switch(card[0]){
		case '2': c.Value = TWO; break;
		case '3': c.Value = THREE; break;
		case '4': c.Value = FOUR; break;
		case '5': c.Value = FIVE; break;
		case '6': c.Value = SIX; break;	
		case '7': c.Value = SEVEN; break;
		case '8': c.Value = EIGHT; break;
		case '9': c.Value = NINE; break;
		case 'T': c.Value = TEN; break;
		case 'J': c.Value = JACK; break;
		case 'Q': c.Value = QUEEN; break;
		case 'K': c.Value = KING; break;
		case 'A': c.Value = ACE; break;
	}
	/* Pull Suit */
	switch(card[1]){
		case 'S': c.Suit = S; break;
		case 'H': c.Suit = H; break;
		case 'D': c.Suit = D; break;
		case 'C': c.Suit = C; break;
	}
	return c;
}


/* Count the number of occurrences of each card 2 through Ace */
void count_cards(struct player *p)
{
	/* Iterate through the cards and increment the counter for each card */
	/* Increment the index of the card's value in card count */
	int i = 0;
	for((*p).cards[i]; i < 5; i++){
		p->card_count[p->cards[i].Value] += 1; 
	}
	
}

/* Return 1 if flush */
int is_flush(struct player *p)
{
	/* Hand is flush if suit of all the cards is the same */
	int i = 0;
	int flush = 1;

	/* Loop through each card and return 0 if one or more indexes are different suits */
	for(i; i < 5; i++){
		if(((*p).cards[i].Suit) != ((*p).cards[i+1].Suit)){
			flush = 0;
		}
	}
	return flush; 
}

/* Return 1 if straight */	
int is_straight(struct player *p)
{
	/* By this stage, the vector must be constructed.
	 * The hand is a straight, if the cards are sequential.
	 * A2345 as well as TJQKA are both valid straights, and Ace 
	 * assumes the value of 1 or 13, but not both. */
	/* If this is a straight, player's card_count for i, i+1, i+2, i+3 and i+4 should be 1. */
	/* If this is a straight, player's card_count for i, i+1, i+2, i+3 and i+4 should be 1. */
	/* Check for A2345, if the hand is A2345, reset the bit for highcard Ace in the player's vector to appropriately reflect that the value of A is not 13. 
	 * 1 << position will generate a bit mask for the position of Ace. Xoring it with the vector will reset the bit.
	 */

	int straight = 1;
	int i = 0;
	/* Check for regular straights, normal values excluding ace */
	for(i; i < 9; i++){
		if((*p).card_count[i] && (*p).card_count[i+1] && (*p).card_count[i+2] && (*p).card_count[i+3] && (*p).card_count[i+4]){
			straight = 1;
		}
	}
	/*Check again but including ace, at the front and back of values */
	if(straight != 1){
		if(p->card_count[12] && p->card_count[3] && p->card_count[2] && p->card_count[1] && p->card_count[0]){
			(*p).vector &= 0UL << 12;
			straight = 1;
		}
	}
	return straight;

}

/* Return 1 if straight flush */
int is_straight_flush(struct player *p)
{
	/* Detect straight flush. A straight flush is nothing but a straight and a flush */
	if(((*p).vector & (1UL << 40) == 1) && ((*p).vector && (1UL << 41) == 1)){return 1;}
	return 0; 
}
		

/* This is the main function that converts the player's hand into weighted unsigned long number. 

 * It is a 55bit vector as shown below (2 is the LSB and StraightFlush is the MSB) */
/* 2, 3, 4, 5, 6, 7, 8, 9, T, J, Q, K, A, 22, 33, 44, 55, 66, 77, 88, 99, TT, JJ, QQ, KK, AA,
 * 222, 333, 444, 555, 666, 777, 888, 999, TTT, JJJ, QQQ, KKK, AAA, Straight, Flush, 2222, 3333, 
 * 4444, 5555, 6666, 7777, 8888, 9999, TTTT, JJJJ, QQQQ, KKKK, AAAA, StraightFlush */

/* The number of occurrences of each number in the hand is first calculated in count_cards. 
 * Then, depending on the numeber of occurrences, the corresponding bit in the bit-vector is set. In order to find the winner, a simple comparison of the bit vectors (i.e., unsigned long integers) will suffice! */
/* Invoke the function to count the occurrences of each card */
/*
 * Iterate through the card_count array, and set the appropriate bit in the bit vector.
 * For example, if the hand is JH JS 3C AH 4H, set the 23rd bit to indicate a pair of jacks, and 2nd, 3rd and 13th bit to indicate 3, 4, and A respectively. */
/* Check if this is a straight, flush or a straight flush, and set the appropriate bit in the vector. */

void eval_strength(struct player *p)
{
	unsigned int straight_or_flush_possible;
	unsigned int i = 0;
	unsigned int pair = 0;
	unsigned int triple = 0;
	straight_or_flush_possible = 1; 
	
	count_cards(p);
	
	/* Set bit vectors */
	for(i; i < 13; i++){
		switch((*p).card_count[i]){
			case 1:
				(*p).vector |= (1 << i);
				break;
			case 2:
				(*p).vector |= (1 << (i+13));
				pair = 1;
				break;
			case 3:
				(*p).vector |= (1 << (i+27));
				triple = 1;
				break;
			case 4:
				(*p).vector |= (1 << (i+43));
				break;
		}
	}

	/*Check for special cases */
	if(is_straight_flush(p)) { (*p).vector |= (1UL << 56);}
	else if(pair && triple) { (*p).vector |= (1UL << 42);}
	else if(is_straight(p)){ (*p).vector |= (1UL << 40);}
	else if(is_flush(p)) { (*p).vector |= (1UL << 41);}	
	
}

void compare_hands(FILE *fp)
{
	/* Parse each hand in the input file, evaluate the strengths of hands and identify a winner by comparing the weighted vectors */
	/* Invoke eval_strength for each player */
	/* Report the winner (e.g., "Player 1 wins") depending on whoever has the higher vector */
	
	char p1[5][3];
	char p2[5][3];
	int i;
	FILE *op;
	op = fopen("Output.txt", "w");

	while(fscanf(fp, "%s %s %s %s %s %s %s %s %s %s", 
		&p1[0][0], &p1[1][0], &p1[2][0], &p1[3][0], &p1[4][0], 
		&p2[0][0], &p2[1][0], &p2[2][0], &p2[3][0], &p2[4][0]) == 10) {
		memset(&P1, 0, sizeof(struct player));
		memset(&P2, 0, sizeof(struct player));
		for(i = 0; i < 5; i++) {
			P1.cards[i] = parse(&p1[i][0]);
			P2.cards[i] = parse(&p2[i][0]);
		}

		eval_strength(&P1);
		eval_strength(&P2);
		
		if(P1.vector >= P2.vector){fprintf(op, "Player 1 wins\n");}
		else{fprintf(op, "Player 2 wins\n");}
		
	/*print output to output.txt */		
	}
	fclose(op);
	fclose(fp);
}

int main(int argc, char *argv[])
{
	/* Validate command line arguments and try to open the file. Exit on failure. */
	/* Invoke the compare hands function */	
	FILE *fp;

	if(argc != 2){
		printf("Invalid number of command line arguments, expects 2.");
		return 1;
	}
 	fp = fopen(argv[1], "r");
	if(fp == (FILE *) NULL){
		printf("File not found: %s", argv[1]);
		return 1;
	}else{
		compare_hands(fp);
	}
	return 0;
}
	
