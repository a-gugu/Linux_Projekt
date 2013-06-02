#ifndef irc_func_cpp__#define irc_func_cpp__//Standard includes#include <iostream>#include <string>#include <cstdlib>#include <cstdio>#include <cstring>#include <fstream>#include <sstream>//Database include#include <sqlite3.h>//Cross platforms configurationa#ifdef WIN32#include <winsock2.h>#else#include <sys/socket.h>#include <sys/types.h>#include <netinet/in.h>#include <arpa/inet.h>#include <netdb.h>#include <unistd.h>#endif#ifdef WIN32SOCKET sockfd;#elseint sockfd;#endif//Include database functions#include "irc_func.h"#include "irc_sqlite3.h"using namespace std;#define MAX_LINE  1024	//Received datasizeint saveLog=0; //switch to on(1) if logs to save or off(2) if not //Create a connection with PORT and HOSTvoid irc_connect(int port, char* host){//Windows platform need to be start #ifdef WIN32	WSADATA wsa;	if(WSAStartup(MAKEWORD(2,0),&wsa) != 0)		exit(1);#endif		const int PORT = port;	const char *HOST = host;	sockfd = socket(AF_INET, SOCK_STREAM, 0);//TCP connection		if(static_cast<int> (sockfd) < 0){		perror("socket()");		irc_disconnect();		exit(1);	}		hostent *hp = gethostbyname(HOST);	if(!hp){		cerr << "gethostbyname()" << endl;		irc_disconnect();		exit(1);	}		//Setup the connection	sockaddr_in sin;	memset((char*)&sin, 0, sizeof(sin));	sin.sin_family = AF_INET;	memcpy((char*)&sin.sin_addr, hp->h_addr, hp->h_length);	sin.sin_port =htons(PORT);	memset(&(sin.sin_zero), 0, 8*sizeof(char));		if(connect(sockfd, (sockaddr*)&sin, sizeof(sin)) == -1){		perror("connenct()");		irc_disconnect();		exit(1);	}}//Cross platforms disconnectionvoid irc_disconnect(){	#ifdef WIN32	closesocket(sockfd);	WSACleanup();#else	close(sockfd);#endif	}//Identify the bot for connection on a server void irc_identify(){		s2u("NICK Superman\r\n");	// Defines the name in channel		s2u("USER Superman 0 0 :Superman\r\n"); // Defines User data		s2u("PRIVMSG NickServ REGISTER Superman\r\n"); // Defines name and password if server support NickServ		s2u("JOIN #Paris\r\n");	// Defines Channel		s2u("Servus Leut�z\r\n"); // Give a meesage in channel}//Received until the connection break//Received data, parse and send back if shouldvoid irc_receive(){		for (;;) {		char buffer[MAX_LINE +1] = {0};				if(recv(sockfd, buffer, MAX_LINE * sizeof(char), 0) < 0) {			perror("recv");			irc_disconnect();			exit(1);		}			irc_parse(buffer);	}}//Send a PONG if the server send PINGvoid ping_parse(const string &buffer){		size_t pingPos = buffer.find("PING");		if(pingPos != string::npos){				string pong("PONG" + buffer.substr(pingPos + 4) + "\r\n");		cout << pong;				s2u(pong.c_str());	}	}//Check and answer if it needsvoid bot_functions(const string &buffer){		size_t pos = 0;		// if message has => Botname: xxx <= then the answer back is => What! I�m busy. <=	if ((pos = buffer.find("Botname: xxx")) != string::npos) {		string tmp(	"PRIVMSG " + buffer.substr((buffer.find(":")+1),(buffer.find("!")-1)) + " What! I�m busy.\r\n");		s2u(tmp.c_str());	}	// if message has => exit <= then the bot shut down himself	else if(buffer.find("exit") != string::npos){		s2u("PRIVMSG #channel :bella ciao ciao\r\n");		irc_disconnect();		exit(0);	}	// if message has => name <= then the bot change his nick to given name params <=	else if((pos = buffer.find("name")) != string::npos){		string tmp = "NICK " + buffer.substr(pos + 6) + "\r\n";		s2u(tmp.c_str());	}	// if message has => savestart <= then the bot start to log in sqlite3 database <=	else if ((pos = buffer.find("savestart")) != string::npos){		saveLog = 1;	}	// if message has => savestop <= then the bot stop to log <=	else if ((pos = buffer.find("savestop")) != string::npos){		saveLog = 0;	}	// if message has => print <= then the bot prints the all datas <=	else if (buffer.find("print") != string::npos) {		sql_getchat();		cout << endl;	}	// if message has => lastseen <= then the prints last seen by user from the log <=	else if (buffer.find("lastseen ") != string::npos) {		sql_lastseen(buffer.substr(buffer.find("lastseen ") + 9,buffer.length() -1).c_str());		cout << endl;	}	// if message has => delete <= then the bot delete the database <=	else if(buffer.find("delete") != string::npos){		sql_delete();	}			if (saveLog) {		//save data in sqlite3 		time_t now = time(0);				time(&now);		tm* localtm = localtime(&now);				//Setup the date and time to a formated string		stringstream t;		t	<< localtm->tm_year+1900	<< "-" << localtm->tm_mon << "-" << localtm->tm_mday << " " 			<< localtm->tm_hour			<< ":" << localtm->tm_min << ":" << localtm->tm_sec;						string name		(buffer.substr(buffer.find(":") +1,		buffer.find("!") -1));	//Get the name		string msg		(buffer.substr(buffer.find(" :") +2,	buffer.length() -1));	//Get the message		string channel	(buffer.substr(buffer.find("!") +1,		buffer.find("@") -1));	//Get the channel name				//Add to database		sql_addchat	(name.c_str(), channel.c_str(), msg.c_str(), t.str().c_str());	}	}//Send the message to uplinkvoid s2u(const char* msg){ send(sockfd, msg, strlen(msg), 0); }//Parse the message delete the unneeded characters//and pass throughvoid irc_parse(string buffer){		if(buffer.find("\r\n") == buffer.length() -2)		buffer.erase(buffer.length() -2);		ping_parse(buffer);		bot_functions(buffer);	}#endif