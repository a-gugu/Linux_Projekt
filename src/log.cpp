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


#ifdef WIN32
	SOCKET sockfd;
#else
	int sockfd;
#endif

#define logFilePath "./logFile.xml"

struct _BotParams {
	char* server, *port, *nick;
} BotParams;

using namespace std;

int loadConfigFile();



int main(){
	
	pid_t pID;
	
	int chat = 1;
	
	cout << "Hi, welcome" << endl;
	
	while (chat > 0) {
		
		if(loadConfigFile() < 0)
			printf("Fail to load config file\n");
		
		pID = fork();
		if (pID < 0) {
			cerr << "Failed to fork" << endl;
			exit(1);
		}
		else if(pID == 0){
			//Child
			cout << "Server: " << BotParams.server << "	" << "Port: " << BotParams.port << "	" << "Nick: " << BotParams.nick << endl;
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
	
	while(0<waitpid(pID,NULL,0));	
	
	return 0;
}


int loadConfigFile(){
	
	printf("Load config file\n");

	FILE *configFile = fopen(logFilePath, "r");
	
	if (configFile == NULL) {
		printf("File not found!");
		fclose(configFile);
		return 0;
	}
	
	char tmp[10][1000];
	char configLine[100];
	int j = 0;
	while (fgets(configLine, 100, configFile) != NULL) {
		sprintf(tmp[j], "%s", configLine);
		j++;
	}
	
	fclose(configFile);
	BotParams.server = tmp[0];
	BotParams.port = tmp[1];
	BotParams.nick = tmp[2];
	/*
	cout << "server:	"	<< tmp[0]	<<	endl;
	cout << "port:	"		<< tmp[1]	<<	endl;
	cout << "nick:	"		<< tmp[2]	<<	endl;
	*/
	return 1;
}