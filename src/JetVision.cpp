#include <signal.h>
#include <cstring>
#include <atomic>

#include "application/App.h"

using namespace std;

atomic<bool> quit = {false};

void gotSignal(int)
{
    quit = true;
}

int main(int argc, char* argv[])
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = gotSignal;
	sigfillset(&sa.sa_mask);
	sigaction(SIGINT,&sa,NULL);

	bool display = false;
	bool debugDisplay = false;

	bool earlyReturn = false;

	if(argc > 1)
	{
		for(int i = 1; i < argc; i++)
		{
			if(strcmp(argv[i], "-d"))
			{
				display = true;
			}
			else if(strcmp(argv[i], "-dd"))
			{
				debugDisplay = true;
			}
			else
			{
				cout << "Unrecognized parameter: " << string(argv[i]) << endl;
				earlyReturn = true;
			}
		}
	}

	if(earlyReturn)
	{
		return 1;
	}


	App *app = new App(display, debugDisplay);
	int result = app->Run(quit);
	delete app;

	return result;
}
