FLAGS = -Wall -pedantic -g

#too lazy to do a good one
all : 
	@rm -f t init *.o *~ \#*
	@ls
	@gcc -c semManip.c $(FLAGS)
	@gcc -c init.c $(FLAGS)
	@gcc -o init init.o semManip.o $(FLAGS)
	@gcc -c typical_process.c $(FLAGS)
	@gcc -o t typical_process.o semManip.o $(FLAGS)



