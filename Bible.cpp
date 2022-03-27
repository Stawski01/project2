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

Bible::Bible()
{ // Default constructor
	infile = "/home/class/csc3004/Bibles/web-complete";
	int i = buildTextIndex();
}

// Constructor – pass bible filename
Bible::Bible(const string s)
{
	infile = s;
	int i = buildTextIndex();
}

// REQUIRED: lookup finds a given verse in this Bible
Verse Bible::lookup(Ref ref, LookupResult &status)
{
	int pos = indexSearch(ref); // Indexed position of ref in file
	Verse aVerse;				// Blank verse
	if (pos != -1)
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
		}
		else // If file is open grabs next line
		{
			instream.seekg(pos, ios::beg); // sets instream to the correct position
			getline(instream, line);	   // Next line
			Verse v(line);				   // Makes line into verse v
			status = SUCCESS;
			return v;
		}
	}

	if (ref.comparison(aVerse.getRef()) == NO_BOOK) // No Chapter error
	{
		status = NO_CHAPTER;
		cerr << error(status) << ref.getChap() << endl;
	}
	else if (ref.comparison(aVerse.getRef()) == NO_CHAPTER) // No Verse error
	{
		status = NO_VERSE;
		cerr << error(status) << ref.getVerse() << endl;
	}
	else if (ref.comparison(aVerse.getRef()) == NO_VERSE) // No Verse error
	{
		status = NO_VERSE;
		cerr << error(status) << ref.getVerse() << endl;
	}
	else
	{
		status = OTHER;
	}

	return (aVerse);
}
// REQUIRED: Return the next verse from the Bible file stream if the file is open.
// If the file is not open, open the file and return the first verse.
// Crete verse from line.
Verse Bible::nextVerse(int pos, LookupResult &status)
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
		instream.seekg(pos, ios::beg); // sets instream to the correct position
		getline(instream, line);	   // Next line
		Verse v(line);				   // Makes line into verse v
		return v;					   // returns verse
	}
}

// If 1 then True if 0 then False.
int Bible::buildTextIndex()
{
	// instream - input file descriptor
	int position; /* location of line in the file */
	// int refcounter = 0; // Reference count
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
			refMap[Ref(line)] = position;
			// refcounter++;
			// cout << refcounter << " : " << position << endl;
		}
	}
	instream.close();
	return 1; // True if it returns a 1.
}

int Bible::indexSearch(Ref r)
{
	map<Ref, int>::iterator it; // iterator for find
	// int blank;					// return for no matches

	/* return the refs */
	/* First use find, so as to NOT create a new entry in refs */
	it = refMap.find(r);
	if (it == refMap.end())
	{
		return -1; // -1 position if couldn't get a value.
	}
	else
	{
		return refMap[r]; // Position of the final ref
	}
}
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
Ref Bible::next(const Ref ref, LookupResult &status)
{
	map<Ref, int>::iterator it; // iterator for find
	Ref blank;					// return for no matches
	/* return the refs */
	/* First use find, so as to NOT create a new entry in refs */
	it = refMap.find(ref); // gets the position of ref in map
	it++;				   // goes to next position in map
	if (it == refMap.end())
	{
		return blank; // empty return
	}
	else
	{
		return it->first; // next ref returned
	}
}

// OPTIONAL: Return the reference before the given ref
// Ref Bible::prev(const Ref ref, LookupResult &status) {}
