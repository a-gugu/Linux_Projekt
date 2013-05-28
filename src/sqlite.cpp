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

#ifdef WIN32
	SOCKET sockfd;
#else
	int sockfd;
#endif

#define SQLITEFILE "../sqlite/loggin.sqlite3"
sqlite3 *sqlitedb = NULL;

struct _BotParams {
	char* server, *port, *nick;
} BotParams;

using namespace std;

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
			//if(execlp ("./ircbot", "./ircbot", BotParams.port, BotParams.server, NULL))
			if(execlp ("./ircbot", "./ircbot", "6667", "irc.europa-irc.de", NULL))
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
	
	cout << "Print datas" << endl;
	while (sqlite3_step(vm) != SQLITE_DONE) {
		char tmp[1200];
		sprintf(tmp, "%s (%d - %s - %s)", sqlite3_column_text(vm, 1), sqlite3_column_int(vm, 2), sqlite3_column_text(vm, 3), sqlite3_column_text(vm, 4));
		cout << sqlite3_column_text(vm, 1) << endl;
	}
	cout << "End printing datas" << endl;
	sqlite3_finalize(vm);
	
	return 1;
}






