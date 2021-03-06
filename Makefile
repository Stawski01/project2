# Bibleajax program
# Bob Kasper, MVNU Computer Science
# updated January 2020 to use
# c++11 compiler option, current paths on cs.mvnu.edu

# This is a Makefile for the Bible web app demo program.
# Copy this directory to a location within your home directory. 
# Change the USER name value below to your own user name.
# Then use "make" to build the server program,
# and deploy it to the live web server directory.
# To test the program, go to http://cs.mvnu.edu/class/csc3004/USER/
# and open the bibleajax.html link.

# TO DO: Must replace "username" by your username
USER= wilsweetman

# Use GNU C++ compiler with C++11 standard
CC= g++
CFLAGS= -g -std=c++11

all:	wblookupclient wblookupserver PutCGI PutHTML

wblookupclient.o:	wblookupclient.cpp fifo.h Ref.h Verse.h Bible.h logfile.h
	$(CC) $(CFLAGS) -c wblookupclient.cpp
# -l option is necessary to link with cgicc library

wblookupclient:	wblookupclient.o fifo.o Ref.o Verse.o Bible.o 
	$(CC) $(CFLAGS) -o wblookupclient wblookupclient.o fifo.o Ref.o Verse.o Bible.o -L/usr/local/lib -lcgicc


wblookupserver.o: wblookupserver.cpp fifo.h Ref.h Verse.h Bible.h
	$(CC) $(CFLAGS) -c wblookupserver.cpp	

wblookupserver: wblookupserver.o fifo.o Ref.o Verse.o Bible.o 
	$(CC) $(CFLAGS) -o wblookupserver wblookupserver.o fifo.o Ref.o Verse.o Bible.o 

fifo.o: fifo.cpp fifo.h
	$(CC) $(CFLAGS) -c fifo.cpp

testreader.o : Ref.h Verse.h Bible.h testreader.cpp
	$(CC) $(CFLAGS) -c testreader.cpp

# # Build the executable
# testreader: Ref.o Verse.o Bible.o testreader.o
# 	$(CC) $(CFLAGS) -o testreader Ref.o Verse.o Bible.o testreader.o


# bibleajax.cgi:	bibleajax.o Ref.o Verse.o Bible.o
# 		$(CC) $(CFLAGS) -o bibleajax.cgi bibleajax.o Ref.o Verse.o Bible.o -lcgicc
# 		# -l option is necessary to link with cgicc library

# main program to handle AJAX/CGI requests for Bible references


# bibleajax.o:	bibleajax.cpp
# 		$(CC) $(CFLAGS) -c bibleajax.cpp

# Ref Object
Ref.o : Ref.h Ref.cpp
	$(CC) $(CFLAGS) -c Ref.cpp

# Verse Object
Verse.o : Ref.h Verse.h Verse.cpp
	$(CC) $(CFLAGS) -c Verse.cpp

# Bible Object
Bible.o : Ref.h Verse.h Bible.h Bible.cpp
	$(CC) $(CFLAGS) -c Bible.cpp

			
# PutCGI:	bibleajax.cgi
# 		chmod 755 bibleajax.cgi
# 		cp bibleajax.cgi /var/www/html/class/csc3004/$(USER)/cgi-bin

# 		echo "Current contents of your cgi-bin directory: "
# 		ls -l /var/www/html/class/csc3004/$(USER)/cgi-bin/

# PutHTML:
# 		cp bibleajax.html /var/www/html/class/csc3004/$(USER)

# 		echo "Current contents of your HTML directory: "
# 		ls -l /var/www/html/class/csc3004/$(USER)

PutCGI: wblookupclient
		chmod 757 wblookupclient
		cp wblookupclient /var/www/html/class/csc3004/$(USER)/cgi-bin
		echo "Current contents of your cgi-bin directory: "
		ls -l /var/www/html/class/csc3004/$(USER)/cgi-bin/

PutHTML: bibleindex.html
		cp bibleindex.html /var/www/html/class/csc3004/$(USER)
		echo "Current contents of your HTML directory: "
		ls -l  /var/www/html/class/csc3004/$(USER)

clean:		
		rm *.o core wblookupclient wblookupserver
