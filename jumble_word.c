#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "jumblew.h"

/* TODO: NEED A JUMBLE TREE FREE ROUTINE */

int
jumble (jumble_tree_t * head)
{
  char jumbled_word[MAX_LENGTH];
  match_list_t *match_head;

  while (1)
    {
      printf ("\n\nJumbled Word: ");
      scanf (" %[^\n]", jumbled_word);
      if (jumbled_word[0] == 'q' && jumbled_word[1] == NUL)
	{
	  printf ("\nUser quit ");
	  break;
	}
      if ((match_head = search_jumble_tree (head, jumbled_word)) == NULL)
	printf ("<no matches>");
      else
	{
//       printf(" [FOUND] %p",match_node);
	  show_matches (match_head);
	}
    }
  return 0;
}

/* Takes void, allocates a jumble_tree_t type pointer, loads the tree. *
 * The head of the tree is returned to the calling function.            */
jumble_tree_t *
load_file (char *wordlist)
{
  FILE *fp;
  char word[MAX_LENGTH];
  long int current_file_position, last_file_position;
  int loading_step_size;
  jumble_tree_t *head; 
  // jumble_tree_t *current_node;

  /* Store start time for runtime stats */
  printf ("\n[Loading] <%s>...\n", wordlist);


  /* Allocate and check if the head pointer */
  head = allocate_new_node ();

  if (node_is_empty (head) == TRUE)
    {
      fprintf (stderr, "Failed ... {Tree head is empty} ");
      return NULL;
    }


  /* Open word list file and check */
  fp = fopen (wordlist, "rb");
  if (fp == NULL)
    {
      perror ("Failed ... {Cannot open} ");
      return NULL;
    }

  /* Get the last file byte offset. To be used for loading feedback */
  fseek (fp, 0, SEEK_END);
  last_file_position = ftell (fp);
  rewind (fp);

  loading_step_size = last_file_position / 100000 + 1;


  /* While there are no more words left in the file *
   * read words and load them into the tree         */
  while (!feof (fp))
    {
//      current_node = head;
      current_file_position = ftell (fp);
      fscanf (fp, " %s", word);

      /* Percentage loading feedback */
      if (current_file_position % loading_step_size == 0)
	printf ("\r\t%5.2f %% Complete",
		((float) current_file_position / last_file_position) * 100);

      /* Feedback Loaded Words */
//           printf("\n[Loading]  %*d -> %*s", NUMBER_FIELD_WIDTH , word_count , STRING_FIELD_WIDTH , word );

      /* If the string word contains invalid characters, skip the word */
      if (scan_for_valid_word (word) == FAIL)
	  continue;
	
      word_count++;
      index_into_jumble_tree (head, word);
    }
    
  fclose (fp);
  printf ("\r\t%5.2f %% Complete", 100.0);

  return head;
}

void
index_into_jumble_tree (jumble_tree_t *head, char *word)
{
  jumble_tree_t *current_node, *temp;
  int index, i;
  char pattern[MAX_LENGTH];

  current_node = head;
  strcpy (pattern, word);
  tolower_word (pattern);
  sort_char (pattern);

  /* Direct index */
  /* While end of word (string) not reached advance in the histogram tree */
  for (i = 0; pattern[i] != NUL; i++)
    {
      index = char_to_index (pattern[i]);
      if (current_node->next[index] == NULL)
	{
	  temp = allocate_new_node ();
	  current_node->next[index] = temp;
	}
      current_node = current_node->next[index];
    }
  /* After a word has end, store string */
  add_current_word (current_node, word);
}

/* Searches the tree pointed by head for the unsorted pattern word   *
 * The function sorts the word characters and them searches the tree */
match_list_t *
search_jumble_tree (jumble_tree_t * head, char *word)
{
  char pattern[MAX_LENGTH];
  short int index, i;
  jumble_tree_t *current_node;

  if (node_is_empty (head) == TRUE)
    {
      fprintf (stderr, "\nTree head is empty ");
      return NULL;
    }

/* No need to scan because the index function would return EMPTY if an invalid char is found */
//   if (scan_for_valid_word (word) == FAIL)
//     {
//       printf ("\n{invalid characters in} \"%s\"\n", word);
//       return NULL;
//     }

  current_node = head;
/* Backup original word */
  strcpy (pattern, word);
/* Sort pattern and make lowercase to make it ready to be searched */
  tolower_word (pattern);
  sort_char (pattern);

/* While pattern has not ended, advance in the histogram tree through the links     *
 * whenever the converted index points to null ie no path, stop searching, no match *
 * If the pattern traverses through the tree successfully and the pattern has ended *
 * check if the last node visited is a word end node, which is detected by, if the  *
 * file_pos array stores atleast one word location offset. If yes search succesful  *
 * return match node, else failed return NULL                                       */
  for (i = 0; pattern[i] != NUL; i++)
    {
      index = char_to_index (pattern[i]);
      if (index == EMPTY)
	return NULL;
      if (current_node->next[index] == NULL)
	{
	  return NULL;
	}
      current_node = current_node->next[index];
    }
  return (current_node->match_head);
}

/* NOTE: Shall this function print the match numbers also ? */
void
show_matches (match_list_t * match_head)
{
  match_list_t *current_match;
  short int i;

  for (i = 1, current_match = match_head;
       current_match != (match_list_t *) NULL;
       current_match = current_match->next, i++)
    {
      printf ("\n\t[%d] %s", i, current_match->word);
    }
}


/* Convert a character to index to be used in the historgram_tree -> next array *
 * to move to the next node in the path. Each index is accessed directly        */

short int char_to_index(char c)
{
  short int index;
  
  /* Convert the character to lowercase */
  c = tolower(c);
  if( isalpha (c) ) {
    index = ( c - 'a' );
  }
  else if ( isdigit (c) ) {
    index = ( c - '0' ) + 26;
  }
    else if( c == '\'' ) {
      index = 36;
      }
	else if( c == '-' ) {
	  index = 37;
	}
	  else if( c == '`' ) {
	    index = 38;
	  }
	  else if ( c == '.' ) {
	    index = 39;
	  }
	  else if ( c == '/') {
	    index = 40;
	  }
	  else if ( c == '&' ) {
	    index = 41;
	  }
	  else {
	    /* Not a valid character */
	    return EMPTY;
	  }
  return index;
}

jumble_tree_t *
allocate_new_node (void)
{
  jumble_tree_t *temp;

  /* Allocate memory and check */
  temp = (jumble_tree_t *) malloc (sizeof (jumble_tree_t));
  if (node_is_empty (temp) == TRUE)
    {
      perror ("Cannot allocate memory ... Terminating ");
      exit (1);
    }
    
  /* Allocate all the direct index locations */
  temp->next = (jumble_tree_t **) calloc (sizeof (jumble_tree_t *), MAX_CHAR_SET);  
  if (temp->next == NULL)
  {
    printf ("Cannot allocate memory ... Terminating ");
    exit (1);
  }

  /* Make the match head NULL */
  temp->match_head = NULL;

  return temp;
}


/* Unallocate TEST functions */
void 
free_match_list (match_list_t *head)
{
  match_list_t *temp;
  
  while (head!=NULL)
  {
    temp = head;
    head = head->next;
    free (temp->word);
    free (temp);
  }
}

void 
free_jumble_tree (jumble_tree_t *head)
{
  int i;
  if (head == NULL)
    return;

  for (i=0; i<MAX_CHAR_SET; i++)
  {
    free_jumble_tree (head->next[i]);
  }
  free_match_list (head->match_head);
  free (head->next);
  free (head);
}
/* Unallocate TEST functions END  */


void
tolower_word (char *word)
{
  int i;

  for (i = 0; word[i] != NUL; i++)
    {
      word[i] = tolower (word[i]);
    }
}

void
sort_char (char *word)
{
  qsort (word, strlen (word), sizeof (char), compare_char);
}

int
compare_char (const void *a, const void *b)
{
  return *((const char *) a) - *((const char *) b);
}

int
scan_for_valid_word (const char *word)
{
  int i;

  /* Only consider word to be valid with length greater than MIX_LENGTH */
  /* NOTE: Keep this commented for faster load, because this will only filter 1 length words,
           this is kept for future development */
  /*if ( strlen (word) < MIN_LENGTH )
     return FAIL;
   */

  for (i = 0; word[i] != NUL; i++)
    {
      if (char_to_index (word[i]) == EMPTY)
	return FAIL;
    }
  return SUCCESS;
}

void
add_current_word (jumble_tree_t *current_node, char *word)
{
  int len;
  match_list_t *temp;

  len = strlen (word);

  temp = (match_list_t *) malloc (sizeof (match_list_t));
  if (temp == NULL)
    {
      perror ("Cannot Allocate match_list_t node");
      exit (1);
    }

  temp->word = (char *) malloc (sizeof (char) * (len + 1));
  if (temp->word == NULL)
    {
      perror ("Cannot Allocate match_list_t -> word");
      exit (1);
    }

  strcpy (temp->word, word);
  temp->next = current_node->match_head;
  current_node->match_head = temp;
}

int
node_is_empty (jumble_tree_t * current_node)
{
  return (current_node == (jumble_tree_t *) NULL);
}
