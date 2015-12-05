#include "App.h"

int main(int argc, char* argv[])
{
	bool display = false;
	bool debugDisplay = false;

	bool earlyReturn = false;

	if(argc > 1)
	{
		for(int i = 1; i < argc; i++)
		{
			if(string(argv[i]) == "-d")
			{
				display = true;
			}
			else if(string(argv[i]) == "-dd")
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
	int result = app->Run();
	delete app;

	return result;
}
