IRC Bot

	ircbot - IRC Bot connect to a IRC server

SYNOPSIS

	ircbot port server name channel

DESCRIPTION
# This is an H1 #
This bot start a socket client connection on port and 
server params and then identify with name and channel. 
If the server try to change you nick by its own then the bot will force it back. 
The bot works with following messages: 	
		print		<== to output the database
 		savestart	<== to start the loggin
 		savestop	<== to stop the loggin
 		lastseen [name]	<== take the name and output the newest date and message
 		join [channel]	<== take the channel and join
 		leave [channel]	<== take the channel and part channel
 		delete		<== delete the sqlite3 database file
 		Botname: xxx	<== output a simple message
 		name [name]	<== change the nick to argument in brackets
 		exit		<== off the irc bot

The bot save log in a sqlite3 database called > chat < filename: log.sqlite3

 
OPTIONS
	
BUGS
	Bugs came sometimes with exceptions like string out of range. 
	Possible solution check if database is open.

AUTHOR
	Alexander Guru m1001243.fh-heidelberg.de



