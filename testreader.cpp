// biblereader.cpp
// main function for Project 1

#include "Ref.h"
#include "Verse.h"
#include "Bible.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main(int argc, char **argv)
{
	// Create Bible object to process the raw text file
	Bible webBible("/home/class/csc3004/Bibles/web-complete");
	if (argc == 4 || argc == 5)
	{

		Verse verse;
		int b = atoi(argv[1]), c = atoi(argv[2]), v = atoi(argv[3]); // assigns values from the command line args
		LookupResult result;
		cout << "Using Bible from: ";
		webBible.display();
		// Prompt for input: get reference to lookup
		// This is good for testing your classes quickly for the first milestone.
		// TODO: your final program should get input from command line arguments instead.
		// Create a reference from the numbers
		Ref ref(b, c, v);

		// Use the Bible object to retrieve the verse by reference
		cout << "Looking up reference: ";
		ref.display();
		cout << endl;

		webBible.buildTextIndex();
		cout << "Reference count: " << refMap.size() << endl; // Size of the Map for number of references.
		map<Ref, int>::iterator test = refMap.find(Ref("66:22:21"));
		if (test == refMap.end())
		{
			cout << "Final byte position: Error" << endl;
		}
		else
		{
			cout << "Final byte position: " << refMap[Ref("66:22:21")] << endl; // Position of the final ref
		}

		/* verse = webBible.lookup(ref, result);
		cout << "Result status: " << result << endl;
		if (result == 0)
		{
			verse.display();
			if (argc == 5 && result == 0)
			{
				for (int i = 1; i < atoi(argv[4]); i++)
				{
					cout << endl;
					webBible.nextVerse(result).display();
				}
			}
			cout << endl;
		} */
	}
	else
	{
		cerr << "Error: " << argv[0] << " needs at least 3 numbers after for Book, chapter, and verse." << endl;
	}
}