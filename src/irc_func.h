#ifndef irc_func_h__
#define irc_func_h__

/*	IRC and received, parse and send functions	 */

#include <string>

void irc_connect(int port, char* host);
void irc_disconnect();
void irc_identify();
void irc_parse(std::string buffer);
void irc_receive();

void s2u(const char* msg);
void ping_parse(const std::string &buffer);
void bot_functions(const std::string &buffer);

#endif