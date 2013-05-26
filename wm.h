/* Will contain data structures related to the external application interface */

/* TODO: Later instead of #defining the paths, make an array and loop until a valid list is not found*/

#define MAX_ALLOWED_CHARS 6
#define CONFIG_FILE_NAME "wm.cfg"


char *default_file[] = {
  "./words.txt",
  "/usr/share/dict/linux.words"
};

typedef struct flag_data
{
  int scrabble:1;
  int jumble:1;
  int word_list_given:1;
  int version:1;
  int help:1;
} option_flags;

typedef struct config_data {
  char letter_weight[26];
  char valid_chars[MAX_ALLOWED_CHARS];
  int min_word_length;
  int max_word_length;
} config_var;

config_var config;