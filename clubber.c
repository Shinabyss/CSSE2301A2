#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_line(FILE* file, unsigned maxChar);

typedef struct Card {
    char suit;
    int rank;
    char rankName;
} Card;

Card string_to_card (char* cardString);
Card deal_hand(FILE* d);
int compare_array(char* a, char* b);
int equal_card(Card a, Card b);
int card_array_length(Card* a);
int card_in_array(Card a, Card* b);
void add_card(Card a, Card* b);
char* card_to_string(Card a);
char* cards_to_string(Card* a);
int compare_card(const void* a, const void* b);
Card* extract_suit(Card* source, char suit);
Card* remove_card(Card* source, Card target);
Card char_to_card (char rank, char suit);
Card lowest_available_club(Card* source);

/*int main (int argc, char* argv[]) 
**      main function of Clubber. Handle argument checking
**
**
**
**
**
**
**
*/
int main (int argc, char* argv[]) {
    Card* hand = malloc(55 * sizeof(Card));
    Card* cardsPlayed = malloc(55 * sizeof(Card));
    if (argc != 3) {
        fprintf(stderr, "Usage: player number_of_players myid\n");
        return 1;
    }
    int playerCount = strtol(argv[1], NULL, 10);
    if ((playerCount < 2) || (playerCount > 4)) {
        fprintf(stderr, "Invalid player count\n");
        return 2;
    }
    char player = argv[2][0];
    if (((player != 'A') && (player != 'B') && (player != 'C') && 
            (player != 'D')) || (strlen(argv[2]) != 1) || 
            ((player - 'A') >= playerCount)) {
        fprintf(stderr, "Invalid player ID\n");
        return 3;
    }

    Card test;
    Card* testextract;
    int i;
    for (i = 0; i < 4; i++) {
        printf("i is %d\n", i);
        test = deal_hand(stdin);
        add_card(test, hand);
    }
    printf("cards before sort is: %s\n", cards_to_string(hand));
    test = lowest_available_club(hand);
    printf("card lowest avail before process is %s\n", card_to_string(test));
    testextract = extract_suit(hand, 'C');
    printf("cards after extraction is: %s\n", cards_to_string(testextract));
    qsort(testextract, card_array_length(testextract), 
            sizeof(Card), compare_card);
    printf("cards after sort is: %s\n", cards_to_string(testextract));
    test = lowest_available_club(testextract);
    printf("card lowest avail is %s\n", card_to_string(test));
    Card toRemove = deal_hand(stdin);
    testextract = remove_card(testextract, toRemove);
    printf("cards after removal is %s\n", cards_to_string(testextract));
    test = lowest_available_club(testextract);
    printf("card lowest avail after removal is %s\n", card_to_string(test));

    return 0;
}

/*int play_game ()
**      main while loop of game, handle termination upon receive
**      message of game end from HUB
**
**
**
**
**
**
*/
int play_game () {
    return 0;
}

/*int new_trick(int playOrder, Card* hand, Card* cardsPlayed)
**      new trick function, handle player algorithm based on
**      game status received from HUB
**
**
**
**
**
**
*/
Card new_trick(int playOrder, Card* hand, Card* cardsPlayed, char leadSuit) {
    qsort(hand, card_array_length(hand), sizeof(Card), compare_card);
    Card* clubsHand = extract_suit(hand, 'C');
    Card* spadeHand = extract_suit(hand, 'S');
    Card* diamHand = extract_suit(hand, 'D');
    Card* heartHand = extract_suit(hand, 'H');
    Card* clubsPlayed = extract_suit(cardsPlayed, 'C');
    Card* spadePlayed = extract_suit(cardsPlayed, 'S');
    Card* diamPlayed = extract_suit(cardsPlayed, 'D');
    Card* heartPlayed = extract_suit(cardsPlayed, 'H');
    if (playOrder == 0) {
        Card lowestClub = lowest_available_club(clubsPlayed);
        if (card_in_array(lowestClub, clubsHand) != -1) {
            return lowestClub;
        } else if (card_array_length(diamHand) != 0) {
            return diamHand[0];
        } else if (card_array_length(heartHand) != 0) {
            return heartHand[0];
        } else if (card_array_length(spadeHand) != 0) {
            return spadeHand[0];
        } else {
            return clubsHand[0];
        }
    } else {
        if ((leadSuit == 'C') || (leadSuit == 'S') || (leadSuit == 'D')
                || (leadSuit == 'H')) {
            Card* leadSuitHand = extract_suit(hand, leadSuit);
            if (card_array_length(leadSuitHand)) {
                return leadSuitHand[0];
            }
        }

    return clubsHand[0];
}

/*Card deal_hand (FILE* f)
**      convert file pointer into Card structure
**
**
**
**
**
**
**
*/
Card deal_hand (FILE* f) {
    char* cardString = get_line(f, 3);
    Card retval = string_to_card(cardString);
    return retval;
}

/*Card string_to_card (char* cardString)
**      convert a string representation of card into
**      Card structure.
**
**
**
**
**
**
*/
Card string_to_card (char* cardString) {
    Card retval;
    if (strlen(cardString) != 2) {
        fprintf(stderr, "convertion failure\n");
        retval.rankName = '\0';
        retval.suit = '\0';
        return retval;
    }
    char rank = cardString[0];
    char suit = cardString[1];
    retval = char_to_card(rank, suit);
    return retval;
}

/*Card char_to_card (char rank, char suit)
**      convert a char information of card into
**      Card structure.
**
**
**
**
**
**
*/
Card char_to_card (char rank, char suit) {
    Card retval;
    if ((rank < '2' || rank > '9') && (rank != 'T') && (rank != 'J') &&
            (rank != 'Q') && (rank != 'K') && (rank != 'A')) {
        fprintf(stderr, "rank value invalid\n");
        retval.rankName = '\0';
        retval.suit = '\0';
        return retval;
    }
    if ((suit != 'C') && (suit != 'D') && (suit != 'S') && (suit != 'H')) {
        fprintf(stderr, "suit value invalid\n");
        retval.rankName = '\0';
        retval.suit = '\0';
        return retval;
    }
    retval.suit = suit;
    retval.rankName = rank;
    if ((rank > '1') && (rank <= '9')) {
        retval.rank = rank - '0';
    } else if (rank == 'T') {
        retval.rank = 10;
    } else if (rank == 'J') {
        retval.rank = 11;
    } else if (rank == 'Q') {
        retval.rank = 12;
    } else if (rank == 'K') {
        retval.rank = 13;
    } else if (rank == 'A') {
        retval.rank = 14;
    } else {
        retval.rank = 0;
    }
    return retval;
}

/*char* get_line(FILE* file, unsigned max)
**      read from a file stream and covert to char*
**      take FILE* to read from and the max number
**      of character to read as arguments
**      will read up to max number of character or
**      \n character which ever is reached first
**
**
**
*/
char* get_line(FILE* file, unsigned max) {
    char* line;
    int i, character;
    line = (char*)malloc((max + 1) * sizeof(char));
    if (file == NULL) {
        printf("File reading failed");
    } else {
        for(i = 0; i < max; i++) {
            character = fgetc(file);
            if (((char)character) == '\n') {
                break;
            } else if (feof(file)) {
                break;
            }
            line[i] = character;
        }
    }
    if(strlen(line) > 0) {
        line[i] = '\0';
        return line;
    } else {
        line[0] = '\0';
        return line;
    }
}

/*int compare_array(char* a, char* b)
**      compare string a and string b
**      return 1 if they are the same
**      return 0 otherwise
**
**
**
**
**
*/
int compare_array(char* a, char* b) {
    int i;
    if (strlen(a) != strlen(b)) {
        return 0;
    }
    for (i = 0; i < strlen(a); i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

/*int equal_card(Card a, Card b)
**      compare card a and card b
**      return 1 if they are the same
**      return 0 otherwise
**
**
**
**
**
*/
int equal_card(Card a, Card b) {
    if (a.suit != b.suit) {
        return 0;
    }
    if (a.rank != b.rank) {
        return 0;
    }
    return 1;
}

/*int card_array_length(Card* cards)
**      return the length of a card array
**
**
**
**
**
**
**
*/
int card_array_length(Card* cards) {
    int retval = 0;
    while (cards[retval].rankName != '\0') {
        retval++;
    }
    return retval;
}

/*int card_in_array(Card targ, Card* dest)
**      return the index of the card in target array
**
**
**
**
**
**
**
*/
int card_in_array(Card targ, Card* dest) {
    int retval;
    for (retval = 0; retval < card_array_length(dest); retval++) {
        if (equal_card(targ, dest[retval])) {
            return retval;
        }
    }
    return -1;
}

/*void add_card(Card targ, Card* dest)
**      add a target card to a card array destination
**      this function operate under the assumption
**      that sufficient memmory has been previously
**      allocated to target string
**
**
**
**
*/
void add_card(Card targ, Card* dest) {
    if (card_in_array(targ, dest) == -1) {
        Card term;
        term.suit = '\0';
        term.rankName = '\0';
        dest[card_array_length(dest)] = targ;
        dest[card_array_length(dest)] = term;
    }
}

/*char* card_to_string(Card card)
**      convert a single card into its string
**      representation
**
**
**
**
**
**
*/
char* card_to_string(Card card) {
    char* retval = malloc(3 * sizeof(char));
    retval[0] = card.rankName;
    retval[1] = card.suit;
    retval[3] = '\0';
    return retval;
}

/*char* cards_to_string(Card* cards)
**      convert an array of cards into a comma
**      separated string
**
**
**
**
**
**
*/
char* cards_to_string(Card* cards) {
    char* retval = malloc(3 * 55 * sizeof(char));
    int i = 0;
    int j = 0;
    retval[i++] = cards[j].rankName;
    retval[i++] = cards[j].suit;
    for (j = 1; j < card_array_length(cards); j++) {
        retval[i++] = ',';
        retval[i++] = cards[j].rankName;
        retval[i++] = cards[j].suit;
    }
    retval[i] = '\0';
    return retval;
}

/*int compare_card(const void* a, const void* b)
**      compares two cards and return a +ve int
**      if a is larger than b, -ve int if b is
**      larger than a and 0 otherwise
**      This function is used in qsort() system
**      function
**
**
**
*/
int compare_card(const void* a, const void* b) {
    const Card* ca = (const Card*) a;
    const Card* cb = (const Card*) b;

    return (ca->rank > cb->rank) - (ca->rank < cb->rank);
}

/*Card* extract_suit(Card* source, char suit)
**      extract all cards of the specified suit
**      from an array of cards
**      return an array of cards containing only
**      the suit specified
**
**
**
**
*/
Card* extract_suit(Card* source, char suit) {
    Card* retval = malloc(55 * sizeof(Card));
    int i;
    int j = 0;
    for (i = 0; i < card_array_length(source); i++) {
        if (source[i].suit == suit) {
            retval[j] = source[i];
            j++;
        }
    }
    retval[j].suit = '\0';
    retval[j].rankName = '\0';
    return retval;
}

/*Card* remove_card(Card* source, Card target)
**      remove a card from a specified card from
**      a card array using equal_card function
**      for comparison.
**      return a Card array that has the target
**      card removed
**
**
**
**
*/
Card* remove_card(Card* source, Card target) {
    Card* retval = malloc(55 * sizeof(Card));
    int i;
    int j = 0;
    for (i = 0; i < card_array_length(source); i++) {
        if (!equal_card(source[i], target)) {
            retval[j] = source[i];
            j++;
        }
    }
    free(source);
    return retval;
}

/*Card lowest_available_club(Card* source)
**      Check an array of cards and return the lowest
**      card of a suit that doesn't exit in the array
**      precondictions are card array contain only clubs
**      
**
**
*/
Card lowest_available_club(Card* source) {
    Card target;
    int i;
    for(i = 0; i < 8; i++) {
        target = char_to_card(('2' + i), 'C');
        if (card_in_array(target, source) == -1) {
            return target;
        }
    }
    target = string_to_card("TC");
    if (card_in_array(target, source) == -1) {
        return target;
    }
    target = string_to_card("JC");
    if (card_in_array(target, source) == -1) {
        return target;
    }
    target = string_to_card("QC");
    if (card_in_array(target, source) == -1) {
        return target;
    }
    target = string_to_card("KC");
    if (card_in_array(target, source) == -1) {
        return target;
    }
    target = string_to_card("AC");
    if (card_in_array(target, source) == -1) {
        return target;
    } else {
        target.rankName = '\0';
        target.suit = '\0';
        return target;
    }
}
