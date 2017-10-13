CC = gcc
PROGNAME = main
SOURCE = structCalculatorMessage.c calc1.c
FLAGS = -Wall

#too lazy to do a good one
all : 
	@gcc -c structCalculatorMessage.c $(FLAGS)
	@gcc -c calc1.c $(FLAGS)
	@gcc calc1.o structCalculatorMessage.o -o calc1 $(FLAGS)
	@gcc -c client1.c $(FLAGS)
	@gcc client1.o structCalculatorMessage.o -o client1 $(FLAGS)

clean :
	@rm -f $(PROGNAME) *.o *~ \#*
	@ls 



