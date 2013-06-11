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

//Path for finding and saving database
#define sqlite3Path "log.sqlite3"

//Pointer to database
sqlite3 *sqlitedb = NULL;


//Initialize the database if not exist
void sql_init(){
	if(sqlite3_open(sqlite3Path, &sqlitedb) != 0)	
		std::cout << "sql_init() :: sqlite3_open()";
	
	sql_createtables();
}

//Close the database
void sql_close(){
	sqlite3_close(sqlitedb);
}

//Create a table for saving nick || channel || chat || date
void sql_createtables(){
	sqlite3_exec(sqlitedb, "CREATE TABLE chat (id integer primary key, nick text, channel text, chat text, date text);", NULL, NULL, NULL);
}

//Drop the table
void sql_delete(){
	sqlite3_exec(sqlitedb, "DROP TABLE chat", NULL, NULL, NULL);
}

//Add a received message
void sql_addchat(const char name[],const char channel[],const char chat[], const char date[]){
	char tmp[1200];
	sprintf(tmp,"INSERT INTO chat (nick,channel, chat, date) VALUES ('%s', '%s', '%s', '%s');",name,channel,chat, date);
	sqlite3_exec(sqlitedb, tmp, NULL, NULL, NULL);
}

//Query by name and output  to uplink
std::string sql_lastseen(const char name[]){
	
	sqlite3_stmt *vm;
	
	std::stringstream s;
	s << "SELECT * FROM chat WHERE nick ='" << name << "' ORDER BY id DESC LIMIT 1";
	sqlite3_prepare(sqlitedb, s.str().c_str(), -1, &vm, NULL);
	
	std::stringstream ss;
	
	while (sqlite3_step(vm) != SQLITE_DONE)
	{
		ss	<< (char*)sqlite3_column_text(vm, 1) << " "
			<< (char*)sqlite3_column_text(vm, 4) << std::endl;
	}
	
	sqlite3_finalize(vm);
	
	return ss.str();
}
//Output the log to uplink
std::string sql_getchat(){
	sqlite3_stmt *vm;
	if(sqlite3_prepare(sqlitedb, "SELECT * FROM chat", -1, &vm, NULL) != 0)
		sql_createtables();
	
	std::stringstream ss;

	while (sqlite3_step(vm) != SQLITE_DONE)
	{
		ss	<< (char*)sqlite3_column_text(vm, 1) << " "
			<< (char*)sqlite3_column_text(vm, 2) << " "
			<< (char*)sqlite3_column_text(vm, 3) << " "
			<< (char*)sqlite3_column_text(vm, 4) << " $ ";
	}
	
	sqlite3_finalize(vm);
	
	return ss.str();
}


#endif
