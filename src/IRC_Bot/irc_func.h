#ifndef irc_func_h__
#define irc_func_h__

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

#include <string.h>

using namespace std;

void irc_connect(int port, char* host);
void irc_disconnect();
void irc_identify();
void irc_parse(string buffer);

inline void s2u(const char* msg){send(sockfd, msg, strlen(msg), 0);}
void ping_parse(const string &buffer);
void bot_functions(const string &buffer);

#endif