#include <LocalIo.h>

int main(int, char **argv)
{
	ProgName(*argv);
    cerr << Error("Test Error") << Exit;

    return 0;
}
