#ifndef JUMBLEW_H
#define JUMBLEW_H

#define a_minus_A 32		//pre calculated value of 'a' - 'A'
#define MAX_LENGTH 150
#define MIN_LENGTH 2
#define MAX_CHAR_SET 42
#define NUMBER_FIELD_WIDTH 10
#define STRING_FIELD_WIDTH -50
#define FILE_NAME_LENGTH 256
#define EMPTY -1
#define SUCCESS 0
#define FAIL 1
#define TRUE 1
#define FALSE 0
#ifndef NUL
#define NUL '\0'
#endif
#ifndef NULL
#define NULL '\0'
#endif

typedef struct match_list_struct
{
  char *word;
  struct match_list_struct *next;
} match_list_t;

typedef struct histogram_structure
{
  match_list_t *match_head;
//   char count;
//   char *next_index;
  struct histogram_structure **next;
} jumble_tree_t;


jumble_tree_t *load_file (char *wordlist);
match_list_t *search_jumble_tree (jumble_tree_t * head, char *word);
void show_matches (match_list_t * match_head);
void add_current_word (jumble_tree_t *current_node, char *word);
void index_into_jumble_tree (jumble_tree_t * head, char *word);
jumble_tree_t *allocate_new_node (void);
short int char_to_index (char c);
void tolower_word (char *word);
void print_information (void);
void sort_char (char *word);
int compare_char (const void *a, const void *b);
void adjust_next_array (jumble_tree_t * current_node, char c);
int scan_for_valid_word (const char *word);
int node_is_empty (jumble_tree_t * current_node);

void free_jumble_tree (jumble_tree_t *head);
void free_match_list (match_list_t *head);

int jumble (jumble_tree_t * head);

long int total_node;
long int word_count;
long int total_bytes;
long int match_list_nodes;
long int realloc_next_index;
double dictinary_loading_time;

//Remove statistics and see performance
#endif