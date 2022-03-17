// biblereader.cpp
// main function for Project 1

// Stuff for pipes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include "fifo.h"

// Stuff for bible
#include "Ref.h"
#include "Verse.h"
#include "Bible.h"
#include <string>
#include <stdio.h>
using namespace std;

int main(int argc, char **argv)
{
	// Create fifo to prepare pipe for sending and receiving
	string receive_pipe = "SSrequest"; // receive pipe name
	string send_pipe = "SSreply";	   // send pipe name
	Fifo recfifo(receive_pipe);		   // Creates receive pipe
	Fifo sendfifo(send_pipe);		   // Creates send pipe

	/* Find a way to move bible class inside loop to use a different bible eache time */
	Bible webBible("/home/class/csc3004/Bibles/web-complete"); // Bible object
	while (true)
	{
		recfifo.openread(); // Opens receive pipe
		Verse verse;		// Verse that will be sent back
		LookupResult result;
		string info_recieved = recfifo.recv(); // Recieve string from client
		Ref ref(info_recieved);				   // Turns string into ref object
		cout << "String sent: " << info_recieved << endl;

		string numberofverses = recfifo.recv();

		cout << "Using Bible from: ";
		webBible.display();

		// Use the Bible object to retrieve the verse by reference
		cout << "Looking up reference: ";
		ref.display();
		cout << endl;

		// cout << "Reference count: " << refMap.size() << endl; // Size of the Map for number of references.
		// cout << "First byte value: " << webBible.indexSearch(Ref("1:1:1")) << endl;
		// cout << "Second byte value: " << webBible.indexSearch(Ref("1:1:2")) << endl;
		// cout << "Third byte value: " << webBible.indexSearch(Ref("1:1:3")) << endl;
		// cout << "Final byte value: " << webBible.indexSearch(Ref("66:22:21")) << endl;
		int pos = webBible.indexSearch(ref); // Position of the verse that is wanted.
		if (pos > -1)
		{
			string message;
			verse = webBible.lookup(pos, ref, result); // Makes the verse
			sendfifo.openwrite();					   // Opens send pipe
			if (result == 0)						   // Outputs if result is a SUCCESS
			{
				message += "<p>Search Type: <b> reference </b></p>";
				message += "<p>Your result: </p><h3>" + bookmap[ref.getBook()] + "</h3><p>" +
						   to_string(ref.getChap()) + ":" + to_string(ref.getVerse()) +
						   "<em> " + verse.getVerse() + "</em>"; // Prepares initial verse refrenced to
				cout << "Sending message." << endl;
				sendfifo.send(message); // Sends first verse

				/* int r = ref.getBook(); // For formatting of multiple verse loop
				int t = stoi(numberofverses);
				cout << "number of verses received: " << numberofverses << endl;
				for (int i = 1; i < t; i++) // Loop for multiple verses
				{
					ref = webBible.next(ref, result); // Sets the verse's ref for formatting display
					int pos = webBible.indexSearch(ref);
					if (pos >= 0)
					{
						verse = webBible.lookup(pos, ref, result); // Gets the next verse in the file

						message = "<br>";
						if (r != ref.getBook()) // if the verse is in a new chapter
						{
							r = ref.getBook();
							message += "<h3>" + bookmap[ref.getBook()] + "</h3>"; // Displays new heading for next chapter
						}
						message += to_string(ref.getChap()) + ":" + to_string(ref.getVerse()) + // Displays specified number of verses.
								   " <em>" + verse.getVerse() + "</em>";
						cout << "Sending message in loop." << endl;
						sendfifo.send(message);
					}
				} */
			}
		}
		else
		{
			cout << "ERROR: Position could not be found for Ref: " << endl;
		}
		sendfifo.send("$end");
		sendfifo.fifoclose();
	}
}