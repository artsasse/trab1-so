scheduler: scheduler.c
	gcc -o scheduler scheduler.c -Wall -lncurses -g

clean:
	rm -f scheduler