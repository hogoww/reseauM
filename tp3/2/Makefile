FLAGS = -Wall -pedantic

#too lazy to do a good one
all : 
	@rm -f init *.o *~ \#*
	@ls
	@gcc -c semManip.c $(FLAGS)
	@gcc -c init.c $(FLAGS)
	@gcc -o init init.o semManip.o $(FLAGS)
	@gcc -o t typical_process.c semManip.o $(FLAGS)



