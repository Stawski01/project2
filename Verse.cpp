// Verse class function definitions
// Computer Science, MVNU

#include "Verse.h"
#include <iostream>
using namespace std;

Verse::Verse()
{ // Default constructor
    verseText = "Uninitialized Verse!";
    verseRef = Ref();
}

// REQUIRED: Parse constructor - pass verse string from file
Verse::Verse(const string s)
{
    string rtext = s;         // Assigns destructable variable
    verseRef = Ref(s);        // Assigns verseRef with Ref constructor
    GetNextToken(rtext, " "); // Makes rtext have only the verse text
    verseText = rtext;        // Assigns verseText with verse string
    // use Ref constructor to create verseRef
    // store the remainder of the string in verseText
}

// REQUIRED: Accessors
// DONE
string Verse::getVerse()
{
    return verseText;
}

Ref Verse::getRef()
{
    return verseRef;
}

// display reference and verse
void Verse::display()
{
    verseRef.display();
    cout << " " << verseText;
}
