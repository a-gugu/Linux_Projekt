

##IRC Bot##

	ircbot - IRC Bot connect to a IRC server

##SYNOPSIS

	ircbot port server name channel

##DESCRIPTION

This bot start a socket client connection on port and  <br />
server params and then identify with name and channel.  <br />
If the server try to change you nick by its own then the bot will force it back.  <br />
The bot works with following messages: 	 <br />
<pre><code> 		print		<== to output the database <br />
 		savestart	<== to start the loggin <br />
 		savestop	<== to stop the loggin <br />
 		lastseen [name]	<== take the name and output the newest date and message <br />
 		join [channel]	<== take the channel and join <br />
 		leave [channel]	<== take the channel and part channel <br />
 		delete		<== delete the sqlite3 database file <br />
 		Botname: xxx	<== output a simple message <br />
 		name [name]	<== change the nick to argument in brackets <br />
 		exit		<== off the irc bot <br />
</code></pre>
The bot save log in a sqlite3 database called > chat < filename: log.sqlite3 <br /><br />
#####No Console output so for that:</br>
In "Bot_Connect.txt" save the bot if it not fails the server and port. Also where he is. <br />
In "Bot_Err.txt" save the bot errors. 

###OPTIONS
	
###BUGS
	Bugs came sometimes with exceptions like string out of range.  <br />
	Possible solution check if database is open. <br />

######AUTHOR
	Alexander Gugu m1001243.fh-heidelberg.de <br />
