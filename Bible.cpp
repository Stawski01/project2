// Bible class function definitions
// Computer Science, MVNU

#include "Ref.h"
#include "Verse.h"
#include "Bible.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

map<Ref, int> refMap;

Bible::Bible()
{ // Default constructor
	infile = "/home/class/csc3004/Bibles/web-complete";
}

// Constructor – pass bible filename
Bible::Bible(const string s) { infile = s; }

// REQUIRED: lookup finds a given verse in this Bible
Verse Bible::lookup(Ref ref, LookupResult &status)
{
	// TODO: scan the file to retrieve the line that holds ref ...
	// update the status variable
	// create and return the verse object
	Verse aVerse(nextVerse(status)); // default verse, to be replaced by a Verse object
									 // that is constructed from a line in the file.
	status = SUCCESS;
	while (ref.comparison(aVerse.getRef()) != 0) // Parse to find the verse in the book
	{
		if (ref.comparison(aVerse.getRef()) == 4) // Continues to parse if 0
		{
			status = OTHER;
			aVerse = nextVerse(status);
		}
		else if (ref.comparison(aVerse.getRef()) == 1) // No Chapter error
		{
			status = NO_CHAPTER;
			cerr << error(status) << ref.getChap() << endl;
			break;
		}
		else if (ref.comparison(aVerse.getRef()) == 2) // No Verse error
		{
			status = NO_VERSE;
			cerr << error(status) << ref.getVerse() << endl;
			break;
		}
		else if (ref.comparison(aVerse.getRef()) == 3) // No Verse error
		{
			status = NO_VERSE;
			cerr << error(status) << ref.getVerse() << endl;
			break;
		}
		status = SUCCESS; // Status only if it leaves the loop naturally.
	}

	return (aVerse);
}
// REQUIRED: Return the next verse from the Bible file stream if the file is open.
// If the file is not open, open the file and return the first verse.
// Crete verse from line.
Verse Bible::nextVerse(LookupResult &status)
{
	if (!isOpen) // Opens file if not open
	{
		instream.open(infile.c_str(), ios::in);
		isOpen = true;
	}
	string line;   // Makes String to capture next line
	if (!instream) // Checks if file is open
	{
		error(OTHER);
		Verse v;
		return v;
	}
	else // If file is open grabs next line
	{
		instream.unsetf(ios::skipws); // Removes white space
		getline(instream, line);	  // Next line
		if (instream.eof())			  // Displays error if file ends
		{
			cerr << error(NO_BOOK) << endl;
			return Verse(line);
		}
		Verse v(line); // Makes line into verse v
		return v;	   // returns verse
	}
}

/*  Word search functionality from shakespearelookup */
/* string Bible::getNextWord(string &line)
 *{
 *	string word = "";
 *	int pos;
 *
 *	if (line.length() > 1)
 *	{
 *		pos = line.find_first_of(".,; :?!"
 *								 "''()");
 *		// get next word
 *		if (pos != 0)
 *		{
 *			word = line.substr(0, pos);
 *		}
 *		// trim the line
 *		if (pos == string::npos)
 *		{
 *			pos = line.length() - 1;
 *		}
 *		line = line.substr(pos + 1, 2000);
 *	}
 *	return word;
 *}
 */
int Bible::buildTextIndex()
{
	// instream - input file descriptor
	int position;		/* location of line in the file */
	int refcounter = 0; // Reference count
	string text, line;
	Verse tempVerse;
	LookupResult tempStatus;
	// int pos;

	instream.open(infile.c_str(), ios::in); // Opens file
	if (!instream)							// If file is not open
	{
		cerr << "Error - can't open input file: " << infile << endl;
		return 0; // false, indicates error
	}
	while (!instream.fail())
	{
		/* Get the file position at beginning of line */
		position = instream.tellg();
		instream.unsetf(ios::skipws); // Removes white space
		getline(instream, line);	  // Get's the line of the file
		/* get the next line */
		if (!instream.eof())
		{
			Ref tempRef(line);
			refMap[tempRef] = position;
			refcounter++;
			cout << refcounter << " : " << position << endl;
		}
	}
	return 1; // True if it returns a 1.
}

/* int Bible::indexSearch(Ref r)
{
} */
// REQUIRED: Return an error message string to describe status
string Bible::error(LookupResult status)
{
	string str = "Bible lookup ERROR: ";
	if (status == NO_BOOK)
		return str + "No book matching given number";
	else if (status == NO_CHAPTER)
		return str + "No Chapter with number: ";
	else if (status == NO_VERSE)
		return str + "No verse with number: ";
	else
		return str + "Other issue";
}

void Bible::display()
{
	cout << "Bible file: " << infile << endl;
}

// OPTIONAL access functions
// OPTIONAL: Return the reference after the given ref
// Ref Bible::next(const Ref ref, LookupResult &status) {}

// OPTIONAL: Return the reference before the given ref
// Ref Bible::prev(const Ref ref, LookupResult &status) {}
