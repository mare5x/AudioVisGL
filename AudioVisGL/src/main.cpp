#include "AudioVis.h"

const char* HELP_STR = "usage: AudioVisGL <path>";


int main(int argc, char* argv[])
{
	if (argc == 2) {
		AudioVis app = AudioVis(argv[1]);
		app.run();
	} else 
		printf("%s\n", HELP_STR);

	return 0;
}