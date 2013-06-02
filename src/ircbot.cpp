#include <stdio.h>
#include <cstdlib>

#include "irc_sqlite3.h"
#include "irc_func.h"

int main(int argc, char *argv[]){
	
	if (argc < 2) {
        fprintf(stderr,"To less arguments\n");
        exit(1);
    }
	
	irc_connect(atoi(argv[1]),argv[2]);
	
	irc_identify();
	
	sql_init();
	
	irc_receive();
	
	sql_close();
	
	irc_disconnect();
	
	return 0;
}