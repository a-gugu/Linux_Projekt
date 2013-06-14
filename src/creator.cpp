#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <sstream>
#include <sys/stat.h>

using namespace std;

typedef struct Ircbot {
	string name;
	string port;
	string server;
	string channel;
};

Ircbot *bot;
pid_t *pID;//Defines max bot process now 10

string& getfile(const string& filename, string& buffer);


void filldatas();


int main(){
	
	unsigned int pidIter = 0; 
	
	filldatas();
	
	//Create processes on counts and start the bot step by step
	//Bots are demons and this program close after init them
	while (pidIter < (sizeof(pID)/sizeof(pID[0]))) {
		
		
		pID[pidIter] = fork();
		
		if (pID[pidIter] < 0) {
			cerr << "Failed to fork" << endl;
			exit(1);
		}
		else if(pID[pidIter] == 0){
			//Child call the bot application
			if(execlp ("./ircbot", "./ircbot", bot[pidIter].port.c_str(), bot[pidIter].server.c_str(), bot[pidIter].name.c_str(), bot[pidIter].channel.c_str(), NULL))
				perror("execvp()");
			
			cout << "Should not show this";
			
			break;
		}
		else {
			//parent
			++pidIter;
		}		
	}
	
	return 0;
}

void filldatas(){
	
	//Parse file to get Bot datas and fill  
	string text;
	getfile("Init_Datas.rtf", text);
    
	//Get number of Bots to be startet{bots number is equal to processes number}
	unsigned int counts = atoi(text.substr(text.find("counts=")+7, text.find(";")-1).c_str());
	
	pID = new pid_t[counts];
	
	bot = new Ircbot[counts];
	
	//Delete unnecessary information in text
	text = text.substr(text.find("Bot"));
	
	text.erase(text.find("ENDE"), text.length());
	
	string line;
    vector<string> user_vec;
	//push every line in a vector for easy parsing
	istringstream is(text);
	while( getline( is, line ) ) {
		if (line.empty())
			continue;
			
		if (line.find("\\")) {
			line.erase(line.find("\\"));
		}
		
		user_vec.push_back( line );
    }
	
	//Parse and fill bots
	int iBot = 0;
	for (unsigned int i = 0; i < user_vec.size(); i++) {
		
		size_t pos;
		
		if (user_vec[i].find("Bot") != string::npos){
				++i;
			if((pos=user_vec[i].find("Name:")) != string::npos){
				bot[iBot].name = user_vec[i].substr(pos+5);
				++i;
			}
			
			if((pos=user_vec[i].find("port:")) != string::npos){
				bot[iBot].port = user_vec[i].substr(pos+5);
				++i;
			}
			
			if((pos=user_vec[i].find("server:")) != string::npos){
				bot[iBot].server = user_vec[i].substr(pos+7);
				++i;
			}
			
			if((pos=user_vec[i].find("channel:")) != string::npos){
				bot[iBot].channel = user_vec[i].substr(pos+8);
			
			}
		
		
		bot[iBot].name.erase(		remove(bot[iBot].name.begin(),		bot[iBot].name.end(),		' '), bot[iBot].name.end());
		bot[iBot].port.erase(		remove(bot[iBot].port.begin(),		bot[iBot].port.end(),		' '), bot[iBot].port.end());
		bot[iBot].server.erase(		remove(bot[iBot].server.begin(),	bot[iBot].server.end(),		' '), bot[iBot].server.end());
		bot[iBot].channel.erase(	remove(bot[iBot].channel.begin(),	bot[iBot].channel.end(),	' '), bot[iBot].channel.end());

		++iBot;
		}
		
	}
	
}
	
//Get Bot configur as whole text
string& getfile(const string& filename, string& buffer) {
    
	ifstream in(filename.c_str(), ios_base::binary | ios_base::ate);
    
	in.exceptions(ios_base::badbit | ios_base::failbit | ios_base::eofbit);
    
	buffer.resize(in.tellg());
    
	in.seekg(0, ios_base::beg);
    
	in.read(&buffer[0], buffer.size());
    
	return buffer;
}









