#ifndef SCRABBLEW_H
#define SCRABBLEW_H
/* ### ESSENTIAL: MAKE CODE CLEANUP */
/* Make separate files and a scrabble utility function files*/


/* TODO: Save the played scrabbles and when wanted by the user show scores in a table or print it in a file */

/*the solution data should only contain the score and a pointer to the match_list_t pointer make it*/
/*NOTE: If two strings are same Example "mdmiied" because of repetition of letters, then the solutions print the same string two times, check if two adjacent strings are same and omit repititions*/
/*NOTE: Only weight sorting of the string is done. This will need to sort the masked string to sort lexically again before the call to solve jumble. To avoid this, first lexically sort scrabble, then index sort by weight into another array, then use the indexes to mask the words. Then the made words will already be sorted and no more sorting will be needed.*/

/*convert all lowercase, remove puncts*/

/* NOTE: Declared in sminorf.c */
// char letter_weight[26] = {1,3,3,2,1,4,2,4,1,8,5,1,3,1,1,3,10,1,1,1,1,4,4,8,4,10};
/*BLANK is zero, blank not supported*/
/*                          {A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z};*/


typedef struct solution_struct
{
  char *word;
  char score;			//possibly recalculate score instead of storing it
  struct solution_struct *next;
} solution_data_t;

typedef struct scrabble_struct
{
  char *string;
  solution_data_t *solution;	//solutions sorted by score
  short int full_length_bonus;	// words with all chars bonus
  char *position_multiple;	// character multiple, like 4th letter triple etc.
  char word_multiple;
} scrabble_data_t;

/* this will sort both the string and weight vector with increasing weight*
 * the weight sorted vector will be used to select jumble strings for best score */


scrabble_data_t *allocate_scrabble (void);
char *make_weight (char *string);
void weight_sort (scrabble_data_t * scrabble);
void make_next_jumble (const scrabble_data_t * wt_sorted_scrabble, char * jumble);//, unsigned int perm_mask, unsigned int max_mask_length);
void show_solutions (const scrabble_data_t * scrabble, int count);
// void add_solution (match_list_t * match_head, scrabble_data_t * scrabble);
void add_solution (scrabble_data_t * solution, char *string);
int get_score (scrabble_data_t * scrabble, char *string);
scrabble_data_t *allocate_scrabble (void);
void  free_scrabble (scrabble_data_t *s);
int get_scrabble (scrabble_data_t *s);
int set_scrabble (scrabble_data_t * s, const char *scrabble,
		  const char *letter_string, const char *word_multiple_str);
int scrabble_length (scrabble_data_t * s);
int solve_scrabble (jumble_tree_t *head, scrabble_data_t *s);

int scrabble (jumble_tree_t * head);


#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 15
#define MAX_MASK_LENGTH 0x00000ff

#define INVALID -1
#define QUIT -2
#define DEFAULT "x"

#define TO_INDEX(x) (x - 'a')

/* The below globals are used with the make_jumble() functions.*/
unsigned int perm_mask;		/* Initialization value = 0xfffffff */
unsigned int max_mask_length;
int min_word_length;	/* Initialization value = 2, later to be loaded form configuration file */


#endif