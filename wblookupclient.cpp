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
#include "Ref.h"
#include "Verse.h"
#include "Bible.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

#define logging // enable log file
#define LOG_FILENAME "/home/class/csc3004/tmp/wilsweetman-wblookupclient.log"
#include "logfile.h"

// Pipes for communication
string receive_pipe = "WBreply";
string send_pipe = "WBrequest";

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

    // Sends the bible type to be searched.
    sendfifo.send(btStr);
    log("Sending bible type: " + btStr);

    recfifo.openread();
    log("Open reply fifo");

    // output the response to the web page
    string message, output, c = "0";
    int bookstorage, chapterstorage;

    /* Single verse lookup */
    message = recfifo.recv(); // Expecting a status number 0-4
    c = message;
    log("result status: " + c); // Logs status
    if (c == "0")               // Checks if status recieved is a succesful status
    {
      message = recfifo.recv(); // Recieves the verse
      log("Reply: " + message); // Logs recieved verse
      if (message != "$end")    // Checks for loop exit
      {
        Verse recverse = Verse(message); // Makes the verse
        Ref ref = recverse.getRef();     // Makes the ref
        bookstorage = ref.getBook();     // Stores book for header
        chapterstorage = ref.getChap();  // Stores chapter for header

        output = "<h3>" + bookmap[ref.getBook()] + "</h3>" +
                 "<h4>" + to_string(ref.getChap()) + "</h4>" +
                 "<p><sup>" + to_string(ref.getVerse()) + "</sup> <em>" + recverse.getVerse() + "</em>"; // Output is prepped for display

        // cout << "<br>";
      }
    }
    else
    {
      output += "<p>Lookup error: " + c + "</p>";
      log("Bible lookup error recieved: " + c);
    }

    /* Loop for multiple verses */
    while (message != "$end") // Loop only exits with valid response
    {
      message = recfifo.recv(); // Expecting a status number 0-4
      c = message;
      log("result status: " + c); // Logs status
      if (c == "0")               // Checks if status recieved is a succesful status
      {
        message = recfifo.recv(); // Recieves the verse
        log("Reply: " + message); // Logs recieved verse
        if (message != "$end")    // Checks for loop exit
        {
          Verse recverse = Verse(message); // Makes the verse
          Ref ref = recverse.getRef();     // Makes the ref

          if (bookstorage != ref.getBook() || chapterstorage != ref.getChap())
          {
            /* New book title */
            if (bookstorage != ref.getBook())
            {
              output += "</p><h3>" + bookmap[ref.getBook()] + "</h3>";
              bookstorage = ref.getBook();
            }

            /* New chapter title */
            if (chapterstorage != ref.getChap())
            {
              output += "</p><h4>" + to_string(ref.getChap()) + "</h4>";
              chapterstorage = ref.getChap();
            }
          }

          output += "<p><sup>" + to_string(ref.getVerse()) + "</sup> <em>" + recverse.getVerse() + "</em>"; // Output is prepped for display

          // cout << output; // displays the output
          //  cout << "<br>";
        }
      }
      else
      {
        output += "<p>Lookup error: " + c + "</p>";
        log("Bible lookup error recieved: " + c);
      }
    }
    cout << output;
    cout << endl; // flush output when done
    recfifo.fifoclose();
    log("close reply fifo");
    sendfifo.fifoclose();
    log("close request fifo");
    return 0;
  }
}