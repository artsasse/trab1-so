LDFLAGS=-lncurses
DEBUG=-g
CCFLAGS=-Wall
PROGRAM=scheduler.c
EXECUTABLE=scheduler

scheduler: scheduler.c
	gcc -o $(EXECUTABLE) $(PROGRAM) $(CCFLAGS) $(LDFLAGS) $(DEBUG)

clean:
	rm -f scheduler

mac: scheduler.c
	gcc -o $(EXECUTABLE) $(PROGRAM) $(CCFLAGS) $(LDFLAGS) $(DEBUG) -I/usr/local/opt/ncurses/include -L/usr/local/opt/ncurses/lib