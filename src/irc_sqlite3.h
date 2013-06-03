#ifndef irc_sqlite3_h__
#define irc_sqlite3_h__

#include <string>

/*	Database functions */

void sql_init();
void sql_close();
void sql_createtables();
void sql_delete();
void sql_addchat(const char name[],const char channel[],const char chat[], const char date[]);
std::string sql_lastseen(const char name[]);
std::string sql_getchat();

#endif