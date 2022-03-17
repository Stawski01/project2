/* sslookupclient.cpp
 * Shakespeare full-text search client:
 * receives an Ajax request from web client
 * extracts search string, and sends request to sslookupserver
 */
#include <iostream>
#include <fstream>
#include <string>

// Stuff for Ajax
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"
using namespace cgicc;

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
using namespace std;

// Stuff for Bible lookup
#include <iostream>
#include <stdio.h>
#include <string.h>

#define logging // enable log file
#define LOG_FILENAME "/home/class/csc3004/tmp/wilsweetman-wblookupclient.log"
#include "logfile.h"

// Pipes for communication
string receive_pipe = "SSreply";
string send_pipe = "SSrequest";

int main()
{

  // prepare the response output,
  // send required header before any other output
  cout << "Content-Type: text/plain\n\n"
       << endl;

#ifdef logging
  logFile.open(LOG_FILENAME, ios::out);
#endif

  Cgicc cgi;

  form_iterator st = cgi.getElement("search_type");       // Search type
  form_iterator bibletype = cgi.getElement("bible_type"); // Bible type
  form_iterator book = cgi.getElement("book");            // Book number
  form_iterator chapter = cgi.getElement("chapter");      // Chapter number
  form_iterator verse = cgi.getElement("verse");          // Verse number
  form_iterator nv = cgi.getElement("num_verse");         // Number of verses total

  bool validInput = false; // initial is false before check
                           // Loop to check the chapter, verse, and number of verses elements.
  if (chapter != cgi.getElements().end() && verse != cgi.getElements().end() && nv != cgi.getElements().end())
  { // Checking if all variable values exist
    int chapterNum = chapter->getIntegerValue();
    int verseNum = verse->getIntegerValue();
    int nvNum = nv->getIntegerValue();
    if (chapterNum > 150)
    {
      cout << "<p>The chapter number (" << chapterNum << ") is too high.</p>" << endl;
    }
    else if (chapterNum <= 0)
    {
      cout << "<p>The chapter must be a positive number.</p>" << endl;
    }
    else if (verseNum > 176)
    {
      cout << "<p>The verse number (" << verseNum << ") is too high.</p>" << endl;
    }
    else if (verseNum <= 0)
    {
      cout << "<p>The verse must be a positive number.</p>" << endl;
    }
    else if (nvNum <= -1)
    {
      cout << "<p>The number of verse/s must be a positive number.</p>" << endl;
    }
    else
    {
      validInput = true; // True if valid
    }
  }

  if (validInput)
  {
    int btNum = bibletype->getIntegerValue(); // Maps the bible type to string for lookup
    string btStr = "web";                     // Initial value of btStr is catch-all
    /* Sets the bible that will be used */
    switch (btNum)
    {
    case 0:
      btStr = "dby";
      break;
    case 1:
      btStr = "kjv";
      break;
    case 2:
      btStr = "web";
      break;
    case 3:
      btStr = "webster";
      break;
    case 4:
      btStr = "ylt";
      break;
    }
    string bible = "/home/class/csc3004/Bibles/" + btStr + "-complete"; // Bible that is used

    string searchString = to_string(book->getIntegerValue()) + ":" +
                          to_string(chapter->getIntegerValue()) + ":" + to_string(verse->getIntegerValue());

    Fifo recfifo(receive_pipe);
    Fifo sendfifo(send_pipe);

    sendfifo.openwrite();
    log("Open request pipe");

    // Call server to get results
    sendfifo.send(searchString);
    log("Request: " + searchString);

    // Sends the number of verses desired.
    string numberverses = to_string(nv->getIntegerValue());
    sendfifo.send(numberverses);
    log("Number of Verses: " + numberverses);

    recfifo.openread();
    log("Open reply fifo");

    // output the response to the web page
    string results = "initial";
    int times = 0;
    while (results != "$end")
    {
      log("Reply: " + results);
      results = recfifo.recv();
      if (results != "$end")
      {
        cout << results;
        if (times++ > 2)
        {
          cout << "<br>";
        }
      }
    }
    cout << endl; // flush output when done
    recfifo.fifoclose();
    log("close reply fifo");
    sendfifo.fifoclose();
    log("close request fifo");
    return 0;
  }
}