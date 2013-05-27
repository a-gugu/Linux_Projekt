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

using namespace std;



int main(){
	string ircName, message;
	int port;
	pid_t pID;
	
	int chat = 1;
	
	cout << "Hi, welcome" << endl;
	
	while (chat > 0) {
		
		cout << "Please type IRC name." << endl;
		cin >> ircName;
		cout << "And now the Port number" << endl;
		cin >> port;
		
		pID = fork();
		if (pID < 0) {
			cerr << "Failed to fork" << endl;
			exit(1);
		}
		else if(pID == 0){
			//Child
			cout << "Call chat with: " << "test " << port << " " << ircName << endl;
			
			ostringstream temp;  //temp as in temporary
			temp<<port;
			
			if(execlp ("./ircbot", "./ircbot", temp.str().c_str(), ircName.c_str(), NULL))
				perror("execvp()");
			
			
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
