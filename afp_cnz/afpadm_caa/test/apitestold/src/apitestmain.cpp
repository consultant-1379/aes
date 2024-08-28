// Main apitest

#include "osf\os.h"
#include "apitest.h"

using namespace std;

int main(int argc, char* argv[])
{
	apitest aptest;

	if (argc > 1)
		{
			aptest.run(argv[1]);
		}
	else
		{
			cout << "Usage: " << argv[0] << " generic | <TQ name>" << endl;
		}
	return 0;
}