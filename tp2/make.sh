$flags = "-Wall -pedantic -std=c99"

#too lazy to do a good one

rm -f $(PROGNAME) *.o *~ \#*

gcc -c structCalculatorMessage.c $(flags)
gcc -c calc1.c $(flags)
gcc calc1.o structCalculatorMessage.o -o calc1 $(flags)
gcc -c client1.c $(flags)
gcc client1.o structCalculatorMessage.o -o client1 $(flags)




