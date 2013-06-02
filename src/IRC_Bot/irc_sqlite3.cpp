#ifndef irc_sqlite3_cpp__
#define irc_sqlite3_cpp__


#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>

#include <sqlite3.h>

#include "irc_sqlite3.h"

#define sqlite3Path "log.sqlite3"
sqlite3 *sqlitedb = NULL;

using namespace std;


void sql_init(){
	if(sqlite3_open(sqlite3Path, &sqlitedb) != 0)	
		cout << "sql_init() :: sqlite3_open()";
	
	sql_createtables();
}
void sql_close(){
	sqlite3_close(sqlitedb);
}
void sql_createtables(){
	sqlite3_exec(sqlitedb, "CREATE TABLE chat (id integer primary key, nick text, channel text, chat text, date text);", NULL, NULL, NULL);
}
void sql_delete(){
	sqlite3_exec(sqlitedb, "DROP TABLE chat", NULL, NULL, NULL);
}
void sql_addchat(const char name[],const char channel[],const char chat[], const char date[]){
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
void sql_getchat(){
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


#endif
