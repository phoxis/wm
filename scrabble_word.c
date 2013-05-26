#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "jumblew.h"
#include "scrabblew.h"

/* TODO: Make memory allocation and free management more organized */
/* FIXME: Shall we store a pointer to the already loaded string in the scrabble structure instead of allocation
 * new memory location and copying the string again?
 */
/* TODO: Make the perm_mask local and pass it through the function.
         This will eliminate the global variable*/

/* 
 *  File Name     : scrabble.c
 *  Function Name : scrabble
 *  Parameters    :
 *                @  (jumble_tree_t *) head
 *  Return Type   : (int)
 *                    # return 0 if success 1 if fail
 *  Globals       : 
 *                @ (unsigned int) perm_mask
 *                    # An integer mask used to select character permutations out
 *                      of a string depending of how many 1's are in which position
 *                      of the mask. Initialization value is 0xfffffff . Should be
 *                      initialized before each scrabble search. This mask is modified
 *                      by the make_jumble () function, and initilized here.
 *                @ (unsigned int) max_mask_length
 *                    # Determines the maximum length of the bitfield of perm_mask to be
 *                      considered while masking. max_mask_length determins the maximum
 *                      string length to be considered and doesnt lets the selection for
 *                      permutation in the make_jumble () function to go beyond.
 *                @ (int) min_word_length
 *                    # Describes the minimum letters that should be in a word to consider
 *                      it a vaild one. This is initilized here to 2, and used in the
 *                      make_jumble () function. Any combination generated in make_jumble ()
 *                      is checked and if the length of the combination is less than or
 *                      equal to min_word_length , it is discarded.
 *  Description   : Recieves a head pointer to the jumble_tree_t, scans strings
 *                  from the user and searches it in the tree pointed by the
 *                  passed head pointer. This is the second level interface to
 *                  drive the scrabble core engine.
 *                 
 *                
 */



static char letter_weight[26] = 
{                             /*  a   b   c   d   e   f   g   h   i   j  */
                                  1,  3,  3,  2,  1,  4,  2,  4,  1,  8,
                                  
                              /*  k   l   m   n   o   p   q   r   s   t  */
                                  5,  1,  3,  1,  1,  3, 10,  1,  1,  1,
                                  
                              /*  u   v   w   x   y   z                  */
                                  1,  4,  4,  8,  4, 10
};


int	
scrabble (jumble_tree_t * head)
{
  scrabble_data_t *s;
  int retval, i;
  
  /* TEMPORARY */
  printf ("\nAssigned letter weights: \n");
  for (i=0; i < 26 ; i++)
  {
    printf ("\n%2c = %2d", 'a' + i, letter_weight[i]);
  }
  
  min_word_length = 3;	/* Define the minimum word length which should be
                         *  considered for a solution 
                         */
  
  /* TODO: allocate the scrabble data once and re use the memory
   * for this we would need to clear the solution list
   */
  while (1)
    {
      s = allocate_scrabble ();
      printf ("\n\n");
      while ((retval = get_scrabble (s)) == INVALID)
	;			/* Make here a good quit detection */
      if (retval == QUIT)
	return 0;
      solve_scrabble (head, s);
      printf ("\n");
      show_solutions (s, 10);
      free_scrabble (s);
    }
  return 0;
}

/* These globals are to be used with solve_scrabble and make_next_jumble */
unsigned int perm_mask, max_mask_length;

int 
solve_scrabble (jumble_tree_t *head, scrabble_data_t *s)
{
 
  match_list_t *list;
  char jumble[MAX_WORD_LENGTH];
  int len;
 
  if (head == NULL)
  {
    fprintf (stderr, "Failed ... {Tree head is empty} ");
    return FAIL;
  }
  
  weight_sort (s);
  len = scrabble_length (s);
  
  perm_mask = 0xffffffff;
  max_mask_length = (0xffffffff & ~(0x1 << len));


/*TODO: free 'list' after adding it. Also free unnecessary solutions in 's', keep the top x solutions
The idea to do this is as following. get the list of solution and append to the linked list. After we have
finished, then at the end we can save the needful inside 's'. For this the complex nature of the add_solution
will be ommited and also the memory will not leak. Try keep the solution list such that only top x solutions are
there, greater would be automatically freed
*/
  while (1)
    {
      make_next_jumble (s, jumble);//, perm_mask, max_mask_length);
      if (jumble[0] == NUL)
	break;
      list = search_jumble_tree (head, jumble);

      while (list != (match_list_t *) NULL)
	{
	  /*add_solution adds the solution score sorted in the linked list of solution_data_t */
//           printf("\n[%s]",list->word);
	  add_solution (s, list->word);
	  list = list->next;
	}
    }
    /* TODO: return something ?? */
    return 1;
}


/*NOTE: Fuction already generates combinations in order of decreasing score, without the letter multiple. Update this so that it also takes consideration the letter multiples, so there is no need to search more whenever we find the first match*/
/*TODO: Implement the permutation generator instead of using normal datatypes */
/* TO SCRABBLE.c */
void
make_next_jumble (const scrabble_data_t * wt_sorted_scrabble, char * jumble)//, unsigned int perm_mask, unsigned int max_mask_length)
{
  unsigned int mask = 0x00000001;
  int s_pos = 0, j_pos = 0;

  do
    {
      s_pos = j_pos = 0;
      mask = 0x00000001;
      while (mask & max_mask_length)
	{
	  if ((perm_mask & mask))
	    {
	      jumble[j_pos] = wt_sorted_scrabble->string[s_pos];
	      j_pos++;
	    }
	  s_pos++;
	  mask <<= 1;
	}
      /*update permutation mask */
      perm_mask--;
      /* do not return r-combinations less than min_word_length (except 0) */
      if ((j_pos >= min_word_length) || (j_pos == 0))
	break;
    }
  while (1);

  jumble[j_pos] = '\0';
}

/* will return the score*/
/*BUG: this function sometimes return wrong score check why? or there is some other problem?*/
int
get_score (scrabble_data_t * scrabble, char *string)
{
  int score = 0;
  int i;
  int scrabble_len, solution_len;
  char *wt_vector;

  scrabble_len = strlen (scrabble->string);
  solution_len = strlen (string);

  /* Normal Score */
  wt_vector = make_weight (string);
  for (i = 0; i < solution_len; i++)
    {
      score = score + wt_vector[i] * scrabble->position_multiple[i];
    }

  /* Adding Letter Multiple */
  score *= scrabble->word_multiple;

  /* Add Full Length Bonus */
  if (scrabble_len == solution_len)
    score += scrabble->full_length_bonus;

  return score;
}

/* This is now returning the input string length , think on this and fix */
/* A prompt to get the scrabble input. Only pass allocated pointer */
int
set_scrabble (scrabble_data_t * s, const char *scrabble,
	      const char *letter_string, const char *word_multiple_str)
{
  char multiple_letter, *temp;
  int word_multiple, letter, multiple;
  int len, i;

  len = strlen (scrabble);

  if (scan_for_valid_word (scrabble) == FAIL)
    {
      printf ("\n{invalid characters in} \"%s\" \n", scrabble);
      return INVALID;
    }

  /* if 'x' is input set default multiple value */
  if (strcmp (word_multiple_str, DEFAULT) == 0)
    {
      word_multiple = 1;	/* Default value */
    }
  else
    {
      word_multiple = strtol (word_multiple_str, &temp, 10);
    }

  if (word_multiple < 1)
    {
      printf
	("\nError in \"Word Multiple\" value: Invalid value \"%d\"\n",
	 word_multiple);
      return INVALID;
    }

  if (strcmp (letter_string, DEFAULT) == 0)
    {
      multiple_letter = 's';	/* Default value */
      letter = 1;		/* Default value */
    }
  else
    {
      /* if 'x' is input set default letter multiple value */
      letter = strtol (letter_string, &temp, 10);
      /*Check is string is already NULL */
      if (temp != NULL)
	multiple_letter = *temp;
      else
	multiple_letter = INVALID;
      /* Ignore any more characters after we have read the letter_multiple character */
    }

  if (letter > len || letter < 1)
    {
      printf
	("\nError in \"Letter Multiple\" string: Invaid Letter value \"%d\"\n",
	 letter);
      return INVALID;
    }

  switch (tolower (multiple_letter))
    {
    case 's':
      multiple = 1;
      break;
    case 'd':
      multiple = 2;
      break;
    case 't':
      multiple = 3;
      break;
    case 'q':
      multiple = 4;
      break;
    case 'x':
      /*A default value */
      multiple = 1;
      break;
    default:
      printf
	("\nError in \"Letter Multiple\" string: Invalid Multiple value \"%c\"\n",
	 multiple_letter);
      return INVALID;
    }
  s->string = (char *) malloc (sizeof (char) * len);
  s->position_multiple = (char *) malloc (sizeof (char) * len);

  if (s->string == NULL)
    {
      perror ("Failed");
      exit (1);
    }
  if (s->position_multiple == NULL)
    {
      perror ("Failed");
      exit (1);
    }

  for (i = 0; i < len; i++)
    {
      s->position_multiple[i] = 1;
    }
  (s->position_multiple[letter - 1]) *= multiple;

  s->word_multiple = word_multiple;

/*this is constant during the program, or will change is configuration is changed*/
/* TODO: We will make a configuration structure where we will store the configuration of the game, like the full length bonus and some other game configuration values.*/

  s->full_length_bonus = 50;
  //s->solution = NULL;

  if (s->string == NULL)
    {
      perror ("string Allocation Error");
      exit (1);
    }
  strcpy (s->string, scrabble);

  return len;
}

char *
make_weight (char *string)
{
  int i, len;
  char *weight;

  len = strlen (string);
  weight = (char *) malloc (sizeof (char) * (len + 1));
  if (weight == NULL)
    {
      perror ("Cannot allocate weight vector");
      exit (1);
    }
  for (i = 0; i < len; i++)
    {
      weight[i] = letter_weight[TO_INDEX (tolower (string[i]))];
    }
  return weight;
}

void
weight_sort (scrabble_data_t * scrabble)
{
  int len = 0, i, j;
  char t_string, t_weight;

  len = strlen (scrabble->string);

  for (i = 1; i < len; i++)
    {
      t_string = scrabble->string[i];
      t_weight = letter_weight[TO_INDEX (scrabble->string[i])];
      //for ( j = i - 1 ; ( j >= 0 ) && ( t_string < ( scrabble -> string[j] ) ) ; j-- ) //to sort by string
      for (j = i - 1; (j >= 0)
	   && (t_weight < (letter_weight[TO_INDEX (scrabble->string[j])]));
	   j--)
	{
	  scrabble->string[j + 1] = scrabble->string[j];
	}
      scrabble->string[j + 1] = t_string;
    }
}

void
show_solutions (const scrabble_data_t * scrabble, int count)
{
  solution_data_t *sol;

  sol = (solution_data_t *) malloc (sizeof (solution_data_t));
  if (sol == NULL)
    {
      perror ("Error allocating memory in show_solutions");
      exit (1);
    }
  sol = scrabble->solution;
  
  /* Skip the dummy node with -1 score value */
  sol = sol->next;
  
  if (sol == NULL)
    {
      printf ("\n\tNo Solutions");
    }
  
  while ((sol != NULL) && (count != 0))
    {
      printf ("\n\t%s   :   %d", sol->word, sol->score);
      sol = sol->next;
      count--;
    }
}

/*TODO: introcuce case checking when strcmp */
/*TODO: Give option to keep only top n solutions */
void
add_solution (scrabble_data_t *s, char *string)
{
  solution_data_t *temp, *current_soln_vector;
  int score;
  
  score = get_score (s, string);
//   printf ("\nSOLn = %s : %d", string, score);
  temp = (solution_data_t *) malloc (sizeof (solution_data_t));
  if (temp == NULL)
    {
      perror ("Cannot allocate solution_data_t temp");
      exit (1);
    }

  temp->word = (char *) malloc (sizeof (char) * strlen (string));
  if (temp->word == NULL)
    {
      perror ("Cannot allocate memory in sorted_insert");
      exit (1);
    }

  strcpy (temp->word, string);
  temp->score = score;
  temp->next = NULL;

  current_soln_vector = s->solution;
  /* Keep score sorted solutions */
  /* NOTE: We have first checked if < than condition and then
   * see == and then strcmp. This makes code redundancy but makes
   * less comparisons, because same solutions will occur less and 
   * when two solns with equal score is encountered then only the strcmp
   * is run, which will avoid executing strcmp in every insertion operation
   * in the linked list
   */
  /* TODO: Shall we reomve the lower scored nodes and only keep
   * the highest score nodes?
   */
  while (current_soln_vector->next != NULL)
  {
    if (current_soln_vector->next->score < score)
      {
	temp->next = current_soln_vector->next;
	current_soln_vector->next = temp;
	return;
      }
    /* If another word with the same score is found 
     * then check if it is a same string existing in the
     * solution vector, if no then add it, if yes, discard.
     */
    else if (current_soln_vector->next->score == score)
    {
      /* If the solution already exists then free temp and break */
      if (strcmp (current_soln_vector->next->word, string) == 0)
	{
	  free (temp->word);
	  free (temp);
	  return;
	}    
      /* If it was a solution with same score then add to the
       * list exactly like the above if 
       */
      else
      {
	temp->next = current_soln_vector->next;
	current_soln_vector->next = temp;
	return;
      }
    }
    current_soln_vector = current_soln_vector->next;
  }
  /* Add at last */
  current_soln_vector->next = temp;
}

scrabble_data_t *
allocate_scrabble (void)
{
  scrabble_data_t *temp;

  temp = (scrabble_data_t *) malloc (sizeof (scrabble_data_t));
  if (temp == NULL)
    {
      perror ("Error: Cannot Allocate memory\n");
      return NULL;
    }
  temp->solution = (solution_data_t *) malloc (sizeof (solution_data_t));
  if (temp->solution == NULL)
  {
    perror ("Error: Cannot Allocate memory\n");
    return NULL;
  }
  temp->solution->next = NULL;
  // allocate this unused memory, because we will free this in the free_scrabble routine in the while loop
  temp->solution->word = (char *) malloc (sizeof (char));
  temp->solution->word[0] = '\0';
  temp->solution->score = -1;
  return temp;
}

/* TEST: Test this function for memory leak */
void 
free_scrabble (scrabble_data_t *s)
{
  solution_data_t *temp, *sol_head;
  
  sol_head = s->solution;
  temp = sol_head;
  while (temp != NULL)
  {
    sol_head = sol_head->next;
    free (temp->word);
    free (temp);
    temp = sol_head;
  }  
  free (s->string);
  free (s->position_multiple);
}

int
get_scrabble (scrabble_data_t *s)
{
  char scrabble[100], letter_string[100], word_multiple_str[100];
  int len;

  printf ("\nEnter Scrabble: ");
  scanf ("%s", scrabble);

  if (strcmp (scrabble, "q") == 0)
    return QUIT;

  printf ("Letter Multiple: ");
  scanf ("%s", letter_string);

  printf ("Word Multiple: ");
  scanf ("%s", word_multiple_str);

  len = set_scrabble (s, scrabble, letter_string, word_multiple_str);

  return len;
}

int
scrabble_length (scrabble_data_t * s)
{
  return strlen (s->string);
}

/* Will return the letter weight, use this instead of direct access */
int
scrabble_letter_weight (char letter)
{
  return ((tolower (letter) >= 'a') && (tolower (letter) <= 'z')) ? letter_weight[letter -'a'] : -1;
}
