#include <main/main.h>
int main(int argc, char* argv[])
{
	if (argc < 2)
		return flick::showHelp();
	return flick::createApp(::getArgs(argc, argv));
};
