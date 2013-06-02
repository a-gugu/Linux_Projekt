#include <stdio.h>
#include <cstdlib>

#include "irc_sqlite3.h"
#include "irc_func.h"
/*
 *This irc bot setup the connection with a given address 
 *	works on >irc.europa-irc.de 6667<
 *	create a sqlite3 database to save and query logs
 *-->function to use
 *		print			<== to output the database
 *		savestart		<== to start the loggin
 *		savestop		<== to stop the loggin
 *		lastseen [name]	<== take the name and output the newest date and message
 *		delete			<== delete the sqlite3 database file
 *		Botname: xxx	<== output a simple message
 *		name [name]		<== change the nick to argument in brackets
 *		exit			<== off the irc bot
 */		

int main(int argc, char *argv[]){
	
	if (argc < 2) {
        fprintf(stderr,"To less arguments\n");
        exit(1);
    }
	
	
	//Create a given connection
	irc_connect(atoi(argv[1]),argv[2]);
	
	//Set identify for irc server connection
	irc_identify();
	
	//Init the sqlite3 database for loggin
	sql_init();
	
	//Received while not break the connection
	irc_receive();
	
	//Close the database
	sql_close();
	
	//Disconnect the client
	irc_disconnect();
	
	return 0;
}