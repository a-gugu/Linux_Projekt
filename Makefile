PROG = MyProg
TRGTS = Makefile_IRCBot Makefile_Process

$(PROG): $(TRGTS)

all: $(TRGTS)

Makefile_IRCBot:
	cd ./src ; make -f Makefile_IRCBot

Makefile_Process:
	cd ./src ; make -f Makefile_Process

clean:
	rm -f *.o *~
	cd ./src ; make clean
	cd ./src ; make clean

