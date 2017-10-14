CC = gcc
PROGNAME = main
SOURCE = structCalculatorMessage.c calc.c
FLAGS = -Wall

#too lazy to do a good one
all : 
	@rm -f $(PROGNAME) *.o *~ \#*
	@ls
	@gcc -c mqcalc.c $(FLAGS)
	@gcc -c calc.c $(FLAGS)
	@gcc calc.o mqcalc.o -o calc $(FLAGS) -lpthread
	@gcc -c query.c $(FLAGS)
	@gcc query.o mqcalc.o -o query $(FLAGS)




