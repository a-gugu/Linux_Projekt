#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>

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


#define ARRAYCOUNT(a) (sizeof a / sizeof a[0])

#define logFilePath "logFile.sqlite"
sqlite3 *sqlitedb = NULL;

using namespace std;

const unsigned int MAX_LINE = 1024;

unsigned int saveLog = 0;

void irc_connect(int port, char* host);
void irc_disconnect();
void irc_identify();
void irc_parse(string buffer);

void s2u(const char* msg);
void ping_parse(const string &buffer);
void bot_functions(const string &buffer);

int loadAndSendToUplinkConfigFile();
int saveInLogFile(string messageLog);


void sql_createtables();
void sql_addchat(const char name[],const char channel[],const char chat[]);
void sql_getchat();

int main(int argc, char *argv[]){
	
	if (argc < 2) {
        fprintf(stderr,"To less argumentsn");
        exit(1);
    }
	
	irc_connect(atoi(argv[1]),argv[2]);
	
	irc_identify();
	
	sqlite3_open(logFilePath, &sqlitedb);
	sql_createtables();
	
	for (;;) {
		char buffer[MAX_LINE +1] = {0};
		
		if(recv(sockfd, buffer, MAX_LINE * sizeof(char), 0) < 0) {
			perror("recv");
			irc_disconnect();
			exit(1);
		}
		
		irc_parse(buffer);
		string tm = buffer;
		string name(tm.substr(tm.find(":")+1, tm.find("!") -1));
		cout << endl << "Name " << name << endl;
		string msg(tm.substr(tm.find(" :")+2,tm.length()-1));
		cout << endl << "Message " << msg << endl;
		if (saveLog) {
			//save data in sqlite
			size_t pos;
			/*if (buffer.find("PRIVMSG ") != string::npos) {
				string name(buffer.substr(buffer.find(":")+1, buffer.find("!")));
				
				if(size_t tmPos = buffer.find(" :") != string::npos) {
					string tm = buffer.substr(pos,tmPos);
				}
				else {
					cout << endl << "---- 90 ----" << endl;
				}

			}*/
		}
		
	}
	sqlite3_close(sqlitedb);
	irc_disconnect();
	
	return 0;
}


void sql_createtables()
{
	sqlite3_exec(sqlitedb, "CREATE TABLE chat (id integer primary key, nick text, channel text,  chat text);", NULL, NULL, NULL);
}

void sql_addchat(const char name[],const char channel[],const char chat[])
{
	char tmp[1200];
	sprintf(tmp,"INSERT INTO chat (nick,channel, chat) VALUES ('%s', '%s', '%s');",name,channel,chat);
	sqlite3_exec(sqlitedb, tmp, NULL, NULL, NULL);
}

void sql_getchat()
{
	sqlite3_stmt *vm;
	sqlite3_prepare(sqlitedb, "SELECT * FROM chat", -1, &vm, NULL);
	
	printf("ID:\tnick\tchannel\tchat\n");
	
	while (sqlite3_step(vm) != SQLITE_DONE)
	{
		printf("%i\t%s\t%s\t%s\n", 
			   sqlite3_column_int(vm, 0), 
			   sqlite3_column_text(vm, 1),
			   sqlite3_column_text(vm, 2),
			   sqlite3_column_text(vm, 3)
			   );
	}
	sqlite3_finalize(vm);
	
}


void irc_connect(int port, char* host){
	
#ifdef WIN32
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,0),&wsa) != 0)
		exit(1);
#endif
	
	const int PORT = port;
	const char *HOST = host;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(static_cast<int> (sockfd) < 0){
		perror("socket()");
		irc_disconnect();
		exit(1);
	}
	
	hostent *hp = gethostbyname(HOST);
	if(!hp){
		cerr << "gethostbyname()" << endl;
		irc_disconnect();
		exit(1);
	}
	
	sockaddr_in sin;
	memset((char*)&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	memcpy((char*)&sin.sin_addr, hp->h_addr, hp->h_length);
	sin.sin_port =htons(PORT);
	memset(&(sin.sin_zero), 0, 8*sizeof(char));
	
	if(connect(sockfd, (sockaddr*)&sin, sizeof(sin)) == -1){
		perror("connenct()");
		irc_disconnect();
		exit(1);
	}
}

void irc_disconnect(){
	
#ifdef WIN32
	closesocket(sockfd);
	WSACleanup();
#else
	close(sockfd);
#endif
	
}

void s2u(const char* msg){
	send(sockfd, msg, strlen(msg), 0);
}

char name[255];
void irc_identify(){
		
	s2u("NICK Frosch78\r\n");
	
	s2u("USER Frosch78 0 0 :Frosch78\r\n");
	
	s2u("PRIVMSG NickServ REGISTER Butter\r\n");
	
	s2u("JOIN #Paris\r\n");
	
	s2u("Servus Leut´z\r\n");
}

void ping_parse(const string &buffer){
	
	size_t pingPos = buffer.find("PING");
	
	if(pingPos != string::npos){
		
		string pong("PONG" + buffer.substr(pingPos + 4) + "\r\n");
		cout << pong;
		
		s2u(pong.c_str());
	}
	
}

void bot_functions(const string &buffer){
	
	size_t pos = 0;
	
	if ((pos = buffer.find("Botname: xxx")) != string::npos) {
		string tmp(	"PRIVMSG " + 
					buffer.substr((buffer.find(":")+1),(buffer.find("!")-1)) + 
					" What! I´m busy.\r\n");
		s2u(tmp.c_str());
	}
	else if(buffer.find("exit") != string::npos){
		s2u("PRIVMSG #channel :Cya\r\n");
		irc_disconnect();
		exit(0);
	}
	else if((pos = buffer.find(":name ")) != string::npos){
		string tmp = "NICK " + buffer.substr(pos + 6) + "\r\n";
		s2u(tmp.c_str());
	}
	
	else if ((pos = buffer.find(":savestart")) != string::npos){
		saveLog = 1;
	}
	else if ((pos = buffer.find(":savestop")) != string::npos){
		saveLog = 0;
	}
		
}

void irc_parse(string buffer){
	
	if(buffer.find("\r\n") == buffer.length() -2)
		buffer.erase(buffer.length() -2);
	
	ping_parse(buffer);
	
	bot_functions(buffer);
	
}

int loadAndSendToUplinkConfigFile(){
	
	FILE *configFile = fopen(logFilePath, "r");
	
	if (configFile == NULL) {
		printf("File not found!");
		fclose(configFile);
		return 0;
	}
	
	char configLine[100];
	while (fgets(configLine, 100, configFile) != NULL) {
		string tmp = configLine;
		tmp+= "\r\n";
		s2u(tmp.c_str());
	}
	
	fclose(configFile);
	
	return 1;
}

int saveInLogFile(string messageLog){

	messageLog += "\r\n";
	ofstream myfile (logFilePath, ios::out | ios::app);
	if (myfile.is_open()) {
		myfile << messageLog;
		myfile.close();
	}
	else {
		cout << "Unable to open file";
		return 0;
	}
	
	return 1;
}

