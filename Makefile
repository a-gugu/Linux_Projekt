
# Installationsverzeichnis
CHECKDIR = /usr/bin/sqlite3

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

install:
	@if [ ! -f $(CHECKDIR) ]; \
		then \
		mkdir tmp; \
		cd tmp; \
		wget http://www.sqlite.org/2013/sqlite-autoconf-3071700.tar.gz; \
		tar xfvz sqlite-autoconfig-3071700.tar.gz; \
		./sqlite-autoconfig-307100/configure; \
		sudo make install; \
		cd..;\
		sudo rm tmp;\
		echo "install SQLITE3 in $(CHECKDIR)"; \
	else \
		echo "SQLITE3 is already install";\
	fi


