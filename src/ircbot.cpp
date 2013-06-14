#include <stdio.h>
#include <cstdlib>
#include <fstream>


#include "irc_sqlite3.h"
#include "irc_func.h"
/*
 *This irc bot setup the connection with a given address in Init_Datas.txt
 *	examples are given there
	
 *-->function to use
 
 *		print			<== to output the database
 *		savestart		<== to start the loggin
 *		savestop		<== to stop the loggin
 *		lastseen [name]	<== take the name and output the newest date and message
 *		join [channel]	<== take the channel and join
 *		leave [channel]	<== take the channel and part channel
 *		delete			<== delete the sqlite3 database file
 *		Botname: xxx	<== output a simple message
 *		name [name]		<== change the nick to argument in brackets
 *		exit			<== off the irc bot
 */		


using namespace std;

int main(int argc, char *argv[]){
	
	if (argc < 4) {
		
		fprintf(stderr,"To less arguments\n");
        exit(1);
    }
	

	//Create a given connection
	irc_connect(atoi(argv[1]),argv[2]);
	
	//Set identify for irc server connection
	irc_identify(argv[3],argv[4]);
	
	ofstream f;
	f.open ("Bot_Connect.txt",std::ofstream::out | std::ofstream::app);
	f << argv[1] << endl << argv[2] << endl << endl;
	f.close();
	
	
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