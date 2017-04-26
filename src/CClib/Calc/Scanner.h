// Scanner class used by calc
#include <iosfwd>

class Scanner
{
private:
	const char *line;
	const char *ptr;	// next character
	
	Scanner(const Scanner&);			// disallowed
	void operator=(const Scanner&);			// disallowed
public:
	Scanner() : line(NULL), ptr(NULL) {}
	~Scanner() {}
	
	void Init(const char *input);
	int NextToken();
	Token current;
	
	int IsEOF() { return ptr==NULL || *ptr == '\0'; }

	std::ostream &PrintMarker(std::ostream &out);
	const char *NextChar() { return ptr; }
	
};
