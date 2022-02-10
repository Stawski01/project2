/* Demo server program for Bible lookup using AJAX/CGI interface
 * By James Skon, Febrary 10, 2011
 * updated by Bob Kasper, January 2020
 * Mount Vernon Nazarene University
 *
 * This sample program works using the cgicc AJAX library to
 * allow live communication between a web page and a program running on the
 * same server that hosts the web server.
 *
 * This program is run by a request from the associated html web document.
 * A Javascript client function invokes an AJAX request,
 * passing the input form data as the standard input string.
 *
 * The cgi.getElement function parses the input string, searching for the matching
 * field name, and returing a "form_iterator" oject, which contains the actual
 * string the user entered into the corresponding field. The actual values can be
 * accessed by dereferencing the form iterator twice, e.g. **verse
 *     refers to the actual string entered in the form's "verse" field.
 */
#include "Ref.h"
#include "Verse.h"
#include "Bible.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

/* Required libraries for AJAX to function */
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"
using namespace cgicc;

int main()
{
  /* A CGI program must send a response header with content type
   * back to the web client before any other output.
   * For an AJAX request, our response is not a complete HTML document,
   * so the response type is just plain text to insert into the web page.
   */
  cout << "Content-Type: text/plain\n\n";

  Cgicc cgi; // create object used to access CGI request data

  // GET THE INPUT DATA
  // browser sends us a string of field name/value pairs from HTML form
  // retrieve the value for each appropriate field name
  form_iterator st = cgi.getElement("search_type");
  form_iterator bibletype = cgi.getElement("bible_type");
  form_iterator book = cgi.getElement("book");
  form_iterator chapter = cgi.getElement("chapter");
  form_iterator verse = cgi.getElement("verse");
  form_iterator nv = cgi.getElement("num_verse");

  // Convert and check input data
  bool validInput = false; // initial is false before check
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

  /* TO DO: OTHER INPUT VALUE CHECKS ARE NEEDED ... but that's up to you! */
  /* All input values are checked in the if loop except the book as that is a drop down */

  /* TO DO: PUT CODE HERE TO CALL YOUR BIBLE CLASS FUNCTIONS
   *        TO LOOK UP THE REQUESTED VERSES
   */
  int btNum = bibletype->getIntegerValue(); // Maps the bible type to string for lookup
  string btStr;
  if (btNum == 0)
    btStr = "dby";
  else if (btNum == 1)
    btStr = "kjv";
  else
    btStr = "web";                                                                        // Catch-all if there is an issue
  LookupResult result;                                                                    // Stores result from the lookup
  Bible webBible("/home/class/csc3004/Bibles/" + btStr + "-complete");                    // Bible that is used
  Ref ref(book->getIntegerValue(), chapter->getIntegerValue(), verse->getIntegerValue()); // Makes ref to search for
  Verse verseString = webBible.lookup(ref, result);                                       // Returns the verse from the search

  /* SEND BACK THE RESULTS
   * Finally we send the result back to the client on the standard output stream
   * in HTML text format.
   * This string will be inserted as is inside a container on the web page,
   * so we must include HTML formatting commands to make things look presentable!
   * << "<em> " << result << endl;
   */
  if (validInput && result == 0) // Only if the input is seen as valid and the result is 0
  {
    cout << "Search Type: <b>" << **st << "</b>" << endl;
    cout << "<p>Your result: </p><h3>"
         << bookmap[ref.getBook()] << "</h3><p>" << ref.getChap() << ":" << ref.getVerse()
         << "<em> " << verseString.getVerse() << "</em>" << endl; // Displays initial verse refrenced to
    int r = ref.getBook();
    for (int i = 1; i < nv->getIntegerValue(); i++) // Output only for multiple lines
    {                                               // if statement to declare chapter header
      cout << "<br>" << endl;
      Verse tempverse = webBible.nextVerse(result); // Gets the next verse in the file
      Ref tempref = tempverse.getRef();             // Sets the verse's ref for formatting display
      if (r != tempref.getBook())                   // if the verse is in a new chapter
      {
        r = tempref.getBook();
        cout << "<h3>" << bookmap[tempref.getBook()] << "</h3>" << endl; // Displays new heading for next chapter
      }
      cout << tempref.getChap() << ":" << tempref.getVerse() // Displays specified number of verses.
           << " <em>" << tempverse.getVerse() << "</em>" << endl;
    }
  }
  else if (result == NO_CHAPTER) // Result status of NO_CHAPTER handling
  {
    cout << "<p>" << webBible.error(result) << ref.getChap() << "</p>" << endl;
  }
  else if (result == NO_VERSE) // Result status of NO_VERSE handling
  {
    cout << "<p>" << webBible.error(result) << ref.getVerse() << "</p>" << endl;
  }
  else // Catch-all if the error is unseen but won't output.
  {
    cout << "<p>Invalid Input: <em>the value that was searched for is unavailable</em></p>" << endl;
  }
  return 0;
}
