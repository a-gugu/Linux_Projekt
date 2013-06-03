#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>


using namespace std;



int main(){
	string ircName, port;//saves irc name and port number as c string for exec
	pid_t pID[10];//Defines max bot process now 10
	
	int chat = 1, pidIter = 0; // switcher for asking another bot params, Iterator for forking
	
	cout << "Hi, welcome" << endl;
	
	while (pidIter < 10) {
		
		if (chat == 0) {
			break;
		}
		
		cout << "Please type IRC name." << endl;
		cin >> ircName;
		cout << "And now the Port number" << endl;
		cin >> port;
		
		pID[pidIter] = fork();
		
		if (pID[pidIter] < 0) {
			cerr << "Failed to fork" << endl;
			exit(1);
		}
		else if(pID[pidIter] == 0){
			//Child call the bot application
			cout << "Call chat with: " << "test " << port << " " << ircName << endl;
			
			if(execlp ("./ircbot", "./ircbot", port.c_str(), ircName.c_str(), NULL))
				perror("execvp()");
			
			cout << "Should not show this";
			break;
		}
		else {
			++pidIter;
			
			while (1) {
			
				//Parent ask for goning on 
				cout << "Start another chat ( Y / N )?" << endl;
				string answer;
				cin >> answer;
				if(answer == "Y" || answer == "y"){
					//Start another chat
					chat = 1;
					break;
				}
				else if(answer == "N" || answer == "n"){
					//Close this api
					chat = 0;
					break;
				}
				else {
					cout << "WHAT !!!!" << endl;
					continue;
				}
			}
		}		
	}
	
	return 0;
}