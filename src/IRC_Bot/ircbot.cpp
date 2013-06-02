#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>


#include "irc_sqlite3.h"
#include "irc_func.h"


using namespace std;


const unsigned int MAX_LINE = 1024;

unsigned int saveLog = 0;

int main(int argc, char *argv[]){
	
	if (argc < 2) {
        fprintf(stderr,"To less argumentsn");
        exit(1);
    }
	
	irc_connect(atoi(argv[1]),argv[2]);
	
	irc_identify();
	
	sql_init();
	
	time_t now = time(0);
	
	for (;;) {
		char buffer[MAX_LINE +1] = {0};
		
		if(recv(sockfd, buffer, MAX_LINE * sizeof(char), 0) < 0) {
			perror("recv");
			irc_disconnect();
			exit(1);
		}
		
		string n = buffer;
		cout << endl << endl << endl << endl << n << endl;
		irc_parse(buffer);
		
		if(n.find("\r\n") == n.length() -2)
			n.erase(n.length() -2);
		
		if (saveLog && (!(n.find("PING") != string::npos))) {
			//save data in sqlite
			
			time(&now);
			tm* localtm = localtime(&now);
			
			stringstream t;
			t	<< localtm->tm_year+1900 << "-" << localtm->tm_mon << "-" << localtm->tm_mday << " " 
				<< localtm->tm_hour << ":" << localtm->tm_min << ":" << localtm->tm_sec;

			string tm = n;
			string name(tm.substr(tm.find(":") +1, tm.find("!") -1));
			string msg(tm.substr(tm.find(" :") +2,tm.length() -1));
			string channel(tm.substr(tm.find("!") +1,tm.find("@") -1));
			sql_addchat(name.c_str(), channel.c_str(), msg.c_str(), t.str().c_str());
			if (tm.find("print") != string::npos) {
				
				cout << "Print" << endl;
				sql_getchat();
				cout << endl;
			}
			if (tm.find(":lastseen ") != string::npos) {
				string ntos(tm.substr(tm.find(":lastseen ") + 10,tm.length() -1));
				
				cout << name.length() << " " << ntos.length() << endl;
				sql_lastseen(ntos.c_str());
				cout << endl;
			}
			if(tm.find(":delete ") != string::npos){
				sql_delete();
			}
			
		}
		
	}
	sql_close();
	irc_disconnect();
	
	return 0;
}