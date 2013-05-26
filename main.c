/* TODO: Good Commandline options to switch to scrabble mode or jumble mode */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jumblew.h"
#include "scrabblew.h"
#include "wm.h"
#include "version.h"


/* 
 *  File Name     : main.c
 *  Function Name : main
 *  Parameters    :
 *                @ (int) argc
 *                @ (char (*)[]) argv
 *  Return Type   : (int)
 *                    # return 0 for success and 1 for fail
 *  Globals       : 
 *                @ (char) (*)[] default_file
 *                    # stores a list of default file lists to be tried if the supplied
 *                     file fails.
 *  Description   : Parses the commandline arguments, sets appropriate flags, loads
 *                  word list, calls jumble or scrabble function. This is the outmost
 *                  driver of the jumble and scrabble function.
 */
int
main (int argc, char *argv[])
{
  char wordlist[FILE_NAME_LENGTH];
  jumble_tree_t *head;
  option_flags flag = { 0, 0, 0, 0, 0 };
  int i, j = 0;

  for (i = 1; i < argc; i++)
    {
      if (argv[i][0] == '-')
	{
	  if ((argv[i][1] == 's') && (argv[i][2] == '\0'))
	    {
	      flag.scrabble = TRUE;
	    }
	  if ((argv[i][1] == 'j') && (argv[i][2] == '\0'))
	    {
	      flag.jumble = TRUE;
	    }
	  if ((argv[i][1] == 'h') && (argv[i][2] == '\0'))
	    {
	      flag.help = TRUE;
	      break;
	    }
	  if ((argv[i][1] == 'v') && (argv[i][2] == '\0'))
	    {
	      flag.version = TRUE;
	      break;
	    }
	}
      else
	{
	  strcpy (wordlist, argv[i]);
	  flag.word_list_given = TRUE;
	}
    }

  if (flag.version)
    {
      print_version ();
      exit (0);
    }

  if (flag.help)
    {
      print_help ();
      exit (0);
    }

  /* If both flags are same, override scrabble_flag */
  if (!(flag.scrabble ^ flag.jumble))
    {
      flag.jumble = FALSE;
      flag.scrabble = TRUE;
    }

  /* If no wordlist file supplied, try from the defaults, if still not found terminate
     If a wordlist file is supplied, just try that one, and if not present terminate
     and dont try defaults */
  while (TRUE)
    {
      if (!flag.word_list_given)
	{
	  /* load defaults here */
	  if (default_file[j] == NULL)
	    break;
	  printf ("\nUsing Default %d\n", j + 1);
	  strcpy (wordlist, default_file[j++]);
	}

      if ((head = load_file (wordlist)) == NULL)
	{
	  //     printf("\nSyntax: %s <wordlist_file>\n",argv[0]);
	  if (flag.word_list_given)
	    break;
	}
      else
	break;
    }

  if (node_is_empty (head))
    {
      printf ("\nNo Wordlist File Found. Terminated.\n");
      exit (0);
    }

  if (flag.scrabble)
  {
    printf ("\n\tScrabble Mode\n\n");
    scrabble (head);
  }
  if (flag.jumble)
  {
    printf ("\n\tJumble Mode\n\n");
    jumble (head);
  }

  free_jumble_tree (head);

  printf ("\nBye\n");
  return 0;
}
