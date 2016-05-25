#include <iostream>
#include <cstring>
#include <sstream>
using namespace std;


#include "ReferenceList.h"


// USER SUPPLIED TYPES:

class Element
{
	char *name;
	char *symbol;
	int Z;
	double A;
public:
	Element() {}
	Element(char *n, char *s, int z, double a)
	{
		name   = strdup(n);
		symbol = strdup(s);
		Z = z;
		A = a;
	}
	
	char* Name()	const { return name; }
	char* Symbol() 	const {return symbol; }
};

typedef char* ElementTag;


// USER SUPPLIED FUNCTIONS:


int IsTag(const ElementTag &tag, const Element &el)
{
	return (  !strcmp((char *)tag, el.Symbol()) ||
					!strcmp((char *)tag, el.Name())  );
}


void GetTags(Element &el, LIST<ElementTag> &TL)
{
	TL.Append((ElementTag)el.Symbol());
	TL.Append((ElementTag)el.Name());
}


int Hash(const ElementTag &tag)
{
	int sum = 0;
	char *ptr = (char *)tag;
	
	while(*ptr)
		sum += *ptr++ + 11;
	return sum;
}
//
// instantiate template virtual function
// otherwise, complies but loader error
template <class elem,class elem_tag>
int REFLIST<elem,elem_tag>::hash(const void* tp)
{ return Hash(*(elem_tag *)tp); }
//
// instantiate template class
//template class REFLIST<Element,ElementTag>;


// TEST PROGRAM:

int main(int, char**)
{
        ostringstream teststream;
	string teststring = "H\n";
	REFLIST<Element,ElementTag> PT(509);
	Element el1("Hydrogen", "H", 1, 1);
	Element el2("Helium", "He", 2, 4);
	LIST<Element> RetList;
	Element* el;
	
	PT.Enter(el1);
	PT.Enter(el2);
	
	PT.Get((ElementTag)"Hydrogen", RetList);
	
	for(RetList.Start(); el = RetList; RetList.Next())
		teststream << el->Symbol() << "\n";
	
	if (teststream.str() == teststring) return 0;
	return 1;
}
