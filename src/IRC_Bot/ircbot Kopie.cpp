#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
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


#define ARRAYCOUNT(a) (sizeof a / sizeof a[0])

#define logFilePath "log.sqlite3"
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
void sql_addchat(const char name[],const char channel[],const char chat[], const char date[]);

void sql_getchat();
void sql_lastseen(const char[]);
void sql_delete();


int main(int argc, char *argv[]){
	
	if (argc < 2) {
        fprintf(stderr,"To less argumentsn");
        exit(1);
    }
	
	irc_connect(atoi(argv[1]),argv[2]);
	
	irc_identify();
	
	int e;
	e = sqlite3_open(logFilePath, &sqlitedb);
	if(e != 0)	cout << "SQL error ";
	
	sql_createtables();
	
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
	sqlite3_close(sqlitedb);
	irc_disconnect();
	
	return 0;
}


void sql_createtables()
{
	sqlite3_exec(sqlitedb, "CREATE TABLE chat (id integer primary key, nick text, channel text, chat text, date text);", NULL, NULL, NULL);
}

void sql_delete()
{
	sqlite3_exec(sqlitedb, "DROP TABLE chat", NULL, NULL, NULL);
}

void sql_addchat(const char name[],const char channel[],const char chat[], const char date[])
{
	char tmp[1200];
	sprintf(tmp,"INSERT INTO chat (nick,channel, chat, date) VALUES ('%s', '%s', '%s', '%s');",name,channel,chat, date);
	sqlite3_exec(sqlitedb, tmp, NULL, NULL, NULL);
}



void sql_lastseen(const char name[]){
	
	sqlite3_stmt *vm;
	
	stringstream s;
	s << "SELECT * FROM chat WHERE nick ='" << name << "' ORDER BY id DESC LIMIT 1";
	sqlite3_prepare(sqlitedb, s.str().c_str(), -1, &vm, NULL);
	while (sqlite3_step(vm) != SQLITE_DONE)
	{
		stringstream ss;
		ss	<< (char*)sqlite3_column_text(vm, 1) << " "
		<< (char*)sqlite3_column_text(vm, 4);
		
		cout << ss.str() << endl;
	}
	sqlite3_finalize(vm);
}

void sql_getchat()
{
	sqlite3_stmt *vm;
	if(sqlite3_prepare(sqlitedb, "SELECT * FROM chat", -1, &vm, NULL) != 0)
		sql_createtables();
	
	while (sqlite3_step(vm) != SQLITE_DONE)
	{
		stringstream ss;
		ss << (char*)sqlite3_column_text(vm, 1) << " "
			 << (char*)sqlite3_column_text(vm, 2) << " "
			 << (char*)sqlite3_column_text(vm, 3) << " "
			<< (char*)sqlite3_column_text(vm, 4);
		
		cout << ss.str() << endl << endl << endl;
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

