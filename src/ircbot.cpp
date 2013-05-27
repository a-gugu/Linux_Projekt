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

using namespace std;

const unsigned int MAX_LINE = 1024; //Größe des Empfangpuffers
const int PORT = 6665;
const char *HOST = "irc.psinet.com";

#ifdef WIN32
SOCKET sockfd;        
#else
int sockfd;
#endif
void irc_connect();
void irc_disconnect();
void s2u(const char *msg);
void ping_parse(const string &buffer);
void irc_parse(string buffer);
void irc_identify();
void bot_functions(const string &buffer);

void irc_connect(){
	
#ifdef WIN32
	//Windows-Socket initialisieren
	WSADATA wsa;
	if( WSAStartup( MAKEWORD( 2, 0 ), &wsa ) != 0)
		exit(1);
#endif
	
	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	
	//Cast nur in Windows nötig
	if (static_cast<int> (sockfd) < 0) {
		perror("socket()");
		irc_disconnect();
		exit(1);
	}
	
	//Adressermittlung
	hostent *hp = gethostbyname(HOST);
	if(!hp){
		cerr << "gethostbyname()" << endl;
		irc_disconnect();
		exit(1);
	}
	
	sockaddr_in sin;
	memset( (char*)&sin, 0, sizeof(sin) );
	sin.sin_family = AF_INET;
	memcpy( (char*)&sin.sin_addr, hp->h_addr, hp->h_length );
	sin.sin_port = htons(PORT);
	memset(&(sin.sin_zero), 0, 8*sizeof(char));
	
	//Verbindungsaufbau
	if (connect(sockfd, (sockaddr*)&sin, sizeof(sin)) == -1) {
		perror("connect()");
		irc_disconnect();
		exit(1);
	}
}

void irc_disconnect() {
#ifdef WIN32
	//Windows-Socket freigeben
	closesocket(sockfd);
	WSACleanup();
#else
	close(sockfd);
#endif
}

void s2u(const char *msg){ //send to uplink
	send(sockfd, msg, strlen(msg), 0);
}

void ping_parse(const string &buffer){
	size_t pingPos = buffer.find("PING");
	
	if (pingPos != string::npos) {
		string pong("PONG" + buffer.substr(pingPos + 4) + "\r\n");
		cout << pong;
		s2u(pong.c_str());
	}
}

void irc_parse(string buffer){
	
	if (buffer.find("\r\n") == (buffer.length() - 2)) {
		buffer.erase(buffer.length() - 2);
		ping_parse(buffer);
		bot_functions(buffer);
	}
}

void irc_identify(){
	
	s2u("NICK Bot\r\n");							//Nickname
	s2u("USER Bot 0 0 :VellasBot\r\n");				//Userdaten
	s2u("PRIVMSG NickServ IDENTIFY password\r\n");	//Identifizierung
	s2u("JOIN #channel\r\n");						//Channel betreten
	s2u("PRIVMSG #channel :Hello!\r\n");			//Begrüßungsnachricht
}

void bot_functions(const string &buffer) {
	
	/*if (buffer.find("Hallo!") != string::npos) {
	 s2u(("PRIVMSG #channel :Hallo!\r\n").c_str());
	 }*/
	
	size_t pos = 0;
	
	if ((pos = buffer.find(":say ")) != string::npos) {
		s2u(("PRIVMSG #channel :" + buffer.substr(pos + 5) + "\r\n").c_str());
	}
	
	if (buffer.find(":User!User@User.user.insiderZ.DE") == 0 && buffer.find("exit") != string::npos) {
		s2u("PRIVMSG #channel :Cya\r\n");
		irc_disconnect();
		exit(0);
	}
	
}

int main() {
	irc_connect();
	
	irc_identify();
	
	for (;;) {
		char buffer[MAX_LINE+1] = {0};
		
		if (recv(sockfd, buffer, MAX_LINE*sizeof(char), 0) < 0) {
			perror("recv()");
			irc_disconnect();
			exit(1);
		}
		
		cout << buffer;
		irc_parse(buffer);
	}
	
	irc_disconnect();
	
}





