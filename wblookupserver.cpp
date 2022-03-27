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
	string receive_pipe = "WBrequest"; // receive pipe name
	string send_pipe = "WBreply";	   // send pipe name
	Fifo recfifo(receive_pipe);		   // Creates receive pipe
	Fifo sendfifo(send_pipe);		   // Creates send pipe

	/* All bible objects */
	Bible dbybible("/home/class/csc3004/Bibles/dby-complete");
	Bible kjvbible("/home/class/csc3004/Bibles/kjv-complete");
	Bible webbible("/home/class/csc3004/Bibles/web-complete");
	Bible websterbible("/home/class/csc3004/Bibles/webster-complete");
	Bible yltbible("/home/class/csc3004/Bibles/ylt-complete");
	while (true)
	{
		Bible *bible;
		recfifo.openread(); // Opens receive pipe
		Verse verse;		// Verse that will be sent back
		LookupResult result;
		string info_recieved = recfifo.recv(); // Recieve string from client
		Ref ref(info_recieved);				   // Turns string into ref object
		cout << "String recieved: " << info_recieved << endl;

		string numberofverses = recfifo.recv();
		string recievedBible = recfifo.recv();

		if (recievedBible == "dby")
			bible = &dbybible;
		else if (recievedBible == "kjv")
			bible = &kjvbible;
		else if (recievedBible == "web")
			bible = &webbible;
		else if (recievedBible == "webster")
			bible = &websterbible;
		else if (recievedBible == "ylt")
			bible = &yltbible;
		else
			bible = &webbible; // Default bible->

		cout << "Using Bible from: ";
		bible->display();

		// Use the Bible object to retrieve the verse by reference
		cout << "Looking up reference: ";
		ref.display();
		cout << endl;

		string message;
		verse = bible->lookup(ref, result); // Makes the verse
		sendfifo.openwrite();				// Opens send pipe
		if (result == 0)					// Outputs if result is a SUCCESS
		{
			sendfifo.send(to_string(result));
			message = to_string(ref.getBook()) + ":" + to_string(ref.getChap()) +
					  ":" + to_string(ref.getVerse()) + " " + verse.getVerse(); // Prepares initial verse refrenced to
			cout << "Sending message:" << message << endl;
			sendfifo.send(message); // Sends first verse
			cout << "sent." << endl;

			int t = stoi(numberofverses);
			cout << "number of verses received: " << numberofverses << endl;
			for (int i = 1; i < t; i++) // Loop for multiple verses
			{
				ref = bible->next(ref, result); // Sets the verse's ref for formatting display
				if (result == 0)
				{
					verse = bible->lookup(ref, result); // Gets the next verse in the file
					if (result == 0)
					{
						sendfifo.send(to_string(result));
						message = to_string(ref.getBook()) + ":" + to_string(ref.getChap()) +
								  ":" + to_string(ref.getVerse()) + " " + verse.getVerse();
						cout << "Sending message in loop #" << i << ": " << message << endl;
						sendfifo.send(message);
					}
					else
					{
						bible->error(result);
						sendfifo.send(to_string(result));
						break;
					}
				}
				else
				{
					bible->error(result);
					sendfifo.send(to_string(result));
					break;
				}
			}
		}
		else
		{
			bible->error(result);
			sendfifo.send(to_string(result));
		}

		sendfifo.send("0");
		sendfifo.send("$end");
		sendfifo.fifoclose();
	}
}