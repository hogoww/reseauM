FLAGS = -Wall -pedantic

#too lazy to do a good one
all : 
	@rm -f l t init *.o *~ \#*
	@ls
	@gcc -c semManip.c $(FLAGS)
	@gcc -c init.c $(FLAGS)
	@gcc -o init init.o semManip.o $(FLAGS)
	@gcc -o t ticket_machine.c semManip.o $(FLAGS)
	@gcc -o l leaving_cars.c semManip.o $(FLAGS)



