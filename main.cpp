#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <map>
#include <DbgHelp.h>
#include <strsafe.h>

#include "dump.h"
#include "log.h"
#include "SimpleProfiler.h"

void Sleep1Sec() {
	Sleep(999);
}

CLog logger;
SimpleProfiler sp;

int main() {
	
	logger.setDirectory(L"log");
	logger.setPrintGroup(LOG_GROUP::LOG_ERROR);

	for (int i = 0; i < 10; i++) {
		sp.profileBegin("for");

		Sleep1Sec();

		sp.profileEnd("for");
		printf("%d\n", i);
	}

	sp.printToFile();


	return 0;
}