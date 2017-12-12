FLAGS = -Wall
POSTFLAGS= -lpthread
all:
	gcc $(FLAGS) -c	listAssoc.c $(POSTFLAGS)
	gcc $(FLAGS) -o a annuaire.c listAssoc.o
	gcc $(FLAGS) -o p pair.c listAssoc.o $(POSTFLAGS)
