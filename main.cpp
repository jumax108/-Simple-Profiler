#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include "SimpleProfiler.h"

void Sleep1Sec() {
	Sleep(1000);
}

int main() {

	SimpleProfiler sp;

	for (int i = 0; i < 10; i++) {
		sp.profileBegin("for");

		Sleep1Sec();

		sp.profileEnd("for");
		printf("%d\n", i);
	}



	sp.printToConsole();

	sp.printToFile();


	return 0;
}