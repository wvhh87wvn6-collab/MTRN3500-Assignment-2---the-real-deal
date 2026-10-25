#include "TMM.h"
// Dont forget
// make UGVModule and NetworkedModule abstract (surely there's a better way to do this - this generates a lot of repeat code)
// actually add mutexes / locks to the SM stuff

int main(void) {

	ThreadManagement^ myTMT = gcnew ThreadManagement();

	myTMT->setupSharedMemory();

	myTMT->threadFunction();

	Console::ReadKey();
	Console::ReadKey();

	return 0;
}

