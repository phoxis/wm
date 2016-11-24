compiler = cc
cflags = -Wall -Wextra -Werror
libs = 
objects = general.o jumble_word.o scrabble_word.o main.o
installpath = /usr/bin

wm : $(objects)
	@$(compiler) -o wm $(objects) $(libs) $(cflags)
	@echo -e "success\nas superuser type 'make install' to install\nor run wm from current directory"

general.o : general.c version.h
	@$(compiler) general.c -c $(cflags) -o general.o
jumble_word.o : jumble_word.c jumblew.h
	@$(compiler) jumble_word.c -c $(cflags) -o jumble_word.o
scrabble_word.o : scrabble_word.c scrabblew.h
	@$(compiler) scrabble_word.c -c $(cflags) -o scrabble_word.o
main.o : main.c jumblew.h scrabblew.h version.h
	@$(compiler) main.c -c $(cflags) -o main.o

	 
.PHONY : clean install remove cleanbak
clean : 
	@rm wm $(objects)
	@echo "cleaned"
install :
	@cp -f wm $(installpath)
	@echo -e "installed in "$(installpath) "\ntype wm in the terminal to run" 
remove :
	@rm -f /usr/bin/wm
	@echo "removed from "$(installpath)
cleanbak :
	@rm -f rm *~
	@echo "backup files cleaned"

