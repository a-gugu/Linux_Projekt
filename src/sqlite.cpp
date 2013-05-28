#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>

#ifdef WIN32
	#include <winsock2.h>
#else
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h>
#endif

#include <sqlite3.h>

using namespace std;

#ifdef WIN32
	SOCKET sockfd;
#else
	int sockfd;
#endif

#define SQLITEFILE "../sqlite/loggin.sqlite3"
sqlite3 *sqlitedb = NULL;

struct _BotParams {
	string server, port, nick, channel;
} BotParams;



int sqlite_getdatabase();



int main(){
	
	pid_t pID;
	
	int chat = 1;
	
	cout << "Hi, welcome" << endl;

	if(sqlite3_open(SQLITEFILE, &sqlitedb)){
		cout << "sqlite3_open()";
		exit(1);
	}
	
	if(!sqlite_getdatabase())
		cout << "sqlite_getdatabase" << endl;
	
	while (chat > 0) {
		
		pID = fork();
		if (pID < 0) {
			cerr << "Failed to fork" << endl;
			exit(1);
		}
		else if(pID == 0){
			//Child
			//cout << "Server: " << BotParams.server << "	" << "Port: " << BotParams.port << "	" << "Nick: " << BotParams.nick << endl;
//++++BUG Begin+++++
			if(execlp ("./ircbot", "./ircbot", BotParams.port.c_str(), BotParams.server.c_str(), NULL))
			//if(execlp ("./ircbot", "./ircbot", "6667", "irc.europa-irc.de", NULL))
				perror("execvp()");
//++++BUG End+++++			
			
			cout << "Should not show this";
			chat = 0;
		}
		else {
			//parent
			while (chat > 0) {
				sleep(1);
				cout << "Start another chat ( Y / N )?" << endl;
				string answer;
				cin >> answer;
				if(answer == "Y" || answer == "y"){
					//Start another chat
					chat = 1;
				}
				else if(answer == "N" || answer == "n"){
					//Close this api
					chat = 0;
				}
				else {
					cout << "WHAT !!!!" << endl;
				}
			}		
		}
	}
	sqlite3_close(sqlitedb);
	while(0<waitpid(pID,NULL,0));	
	
	return 0;
}


int sqlite_getdatabase(){
	sqlite3_stmt *vm;
	sqlite3_prepare(sqlitedb, "SELECT * FROM logginlist", -1, &vm, NULL);
	
	while (sqlite3_step(vm) != SQLITE_DONE) {
		BotParams.server = (char*)sqlite3_column_text(vm, 1);
	
		stringstream ss;
		ss << sqlite3_column_int(vm, 2);
		
		BotParams.port = ss.str();
		
		BotParams.channel = (char*)sqlite3_column_text(vm, 3);
		
		BotParams.nick = (char*)sqlite3_column_text(vm, 4);
	}
	
	//cout << BotParams.server << BotParams.port << BotParams.channel << BotParams.nick;
	
	sqlite3_finalize(vm);
	
	return 1;
}






