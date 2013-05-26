#include <stdio.h>
#include "version.h"

void
print_help (void)
{
  printf ("\nUsage:\n");
  printf ("\twm [-s] [-j] [wordlist]\n");
  printf ("Options: \n");
  printf ("\t-s\tScrabble Mode\n");
  printf ("\t-j\tJumble Mode\n");
  printf ("\t-h\tHelp\n");
  printf ("\t-v\tVersion\n");
  printf ("Description: \n");
  printf ("\tSupply a wordlist file through commands line.\n \t\
If no wordlist file is passed then try to load\n \t\
default file paths. If still not found, terminate\n \t\
If a wordlist file is passed then only try that one.\n");
  printf ("\n");
}

void
print_version (void)
{
  printf ("\n[WordMachine] Version: %s  Revision: %s   Status: %s\n", VERSION,
	  REVISION, STATUS);
}
