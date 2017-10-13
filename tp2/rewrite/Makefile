CC = gcc
PROGNAME = main
SOURCE = structCalculatorMessage.c calc1.c
FLAGS = -Wall

#too lazy to do a good one
all : 
	@rm -f $(PROGNAME) *.o *~ \#*
	@ls 
	@gcc -c mqcalc.c $(FLAGS)
	@gcc -c calc1.c $(FLAGS)
	@gcc calc1.o mqcalc.o -o calc1 $(FLAGS)
	@gcc -c query1.c $(FLAGS)
	@gcc query1.o mqcalc.o -o query1 $(FLAGS)




