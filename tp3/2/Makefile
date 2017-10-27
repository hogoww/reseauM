FLAGS = -Wall -pedantic

#too lazy to do a good one
all : 
	@rm -f init *.o *~ \#*
	@ls
	@gcc -c semManip.c $(FLAGS)
	@gcc -c init.c $(FLAGS)
	@gcc -o init init.o semManip.o $(FLAGS)
	# @gcc -c mqcalc.c $(FLAGS)
	# @gcc -c calc.c $(FLAGS)
	# @gcc calc.o mqcalc.o -o calc $(FLAGS) -lpthread
	# @gcc -c query.c $(FLAGS)
	# @gcc query.o mqcalc.o -o query $(FLAGS)




