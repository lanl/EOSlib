// Simple List of strings class for MakeMake

class List
{
private:
	List(const List&);			// Disallowed
	void operator=(const List&);		// Disallowed
	
	char **list;
	int dim;				// array size
	int n;					// number of elements
	
	void Realloc();
public:
	List(int Max = 32);
	~List();
	
	char* operator[](int i);
	int operator[](const char *);
    // unnecessary, gcc 3.2 bug
    int operator[](char* str)
    {
       const char *s = str;
       return operator[](s);
    }
    //
		
	List& operator<<(const char *);
	void operator=(char *);
	
	void Replace(int i, char *s);
	void Delete();
	
	operator int() const {return n;}
	int Dimension() const { return n; }
};
