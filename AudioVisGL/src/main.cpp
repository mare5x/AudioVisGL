#include "AudioVis.h"

int main(int argc, char* argv[])
{
	if (argc == 2) {
		AudioVis app = AudioVis(argv[1]);
		app.run();
	}
	return 0;
}