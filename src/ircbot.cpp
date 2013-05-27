#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>

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


#define ARRAYCOUNT(a) (sizeof a / sizeof a[0])


using namespace std;

const unsigned int MAX_LINE = 1024;

void irc_connect();
void irc_disconnect();
void irc_identify();
void irc_parse(string buffer);

void s2u(const char* msg);
void ping_parse(const string &buffer);
void bot_functions(const string &buffer);


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
	cout << HOST << ARRAYCOUNT(HOST) << endl;
	hostent *hp = gethostbyname(HOST);
	if(!hp){
		cerr << "gethostbyname()" << "	" << HOST << ARRAYCOUNT(HOST) << endl;
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
	
	printf("\n\tSay my name, say my name\n\tIf no one is around you \n\tSay baby I love you \n\tIf you ain't runnin' game \n");
	
	string message = "NICK ";
	string input;
	
	cin >> input;
	sleep(1);
	s2u((message + input + "\r\n").c_str());
	
	//s2u("NICK Frosch 78\r\n");
	
	s2u("USER Frosch78 0 0 :Frosch78\r\n");
	
	s2u("PRIVMSG NickServ IDENTIFY password\r\n");
	
	s2u("JOIN #channel\r\n");
	
	s2u("PRIVMSG #channel :Froschchannel\r\n");
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
	
	if ((pos = buffer.find(":say ")) != string::npos) {
		s2u(("PRIVMSG #channel:" + buffer.substr(pos + 5) + "\r\n").c_str());
	}
	else if(buffer.find(":User!User@User.user.insiderZ.DE") == 0 && buffer.find("exit") != string::npos){
		s2u("PRIVMSG #channel :Cya\r\n");
		irc_disconnect();
		exit(0);
	}
}

void irc_parse(string buffer){
	
	if(buffer.find("\r\n") == buffer.length() -2)
		buffer.erase(buffer.length() -2);
	
	ping_parse(buffer);
	
	bot_functions(buffer);
	
}

int main(int argc, char *argv[]){
	
	if (argc < 2) {
        fprintf(stderr,"To less argumentsn");
        exit(1);
    }
	cout << endl << endl << endl;
	cout << argv[1] << " " << argv[2];
	cout << endl << endl << endl;
	irc_connect(atoi(argv[1]),argv[2]);//"irc.europa-irc.de"
	
	irc_identify();
	
	for (;;) {
		char buffer[MAX_LINE +1] = {0};
		
		if(recv(sockfd, buffer, MAX_LINE * sizeof(char), 0) < 0) {
			perror("recv");
			irc_disconnect();
			exit(1);
		}
		
		cout << buffer;
		
		irc_parse(buffer);
	}
	
	irc_disconnect();
	
	return 0;
}
