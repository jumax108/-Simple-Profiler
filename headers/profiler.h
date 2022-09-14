#pragma once

#include <stdio.h>
#include <Windows.h>

#include "dump/headers/dump.h"
#if defined _DEBUG
	#pragma comment(lib, "dump/MSVC/debug/dump")
#else
	#pragma comment(lib,"dump/MSVC/release/dump")
#endif

#include "log/headers/log.h"
#if defined _DEBUG
	#pragma comment(lib, "log/MSVC/debug/log")
#else
	#pragma comment(lib,"log/MSVC/release/log")
#endif

#include "common.h"

class CProfiler
{
public:
	struct stProfile;

public:
	
	CProfiler();

	void begin(const char* tag);
	void end(const char* tag);

	stProfile* begin();
	stProfile* next();

	void printToFile();

	void reset();
	
private:

	stProfile* _profile;
	unsigned int _allocIndex = 0;

	LARGE_INTEGER _freq;
	CLog _logger;

	int _returnIndex;

	bool _reset;

	int findIdx(const char* name);
};

struct CProfiler::stProfile {

	stProfile() {

		ZeroMemory(_tag, sizeof(_tag));
		ZeroMemory(&_start, sizeof(LARGE_INTEGER));

		_sum = 0;
		_max = 0;
		_min = 0x7FFFFFFFFFFFFFFF;
		_callCnt = 0;

	}

	char _tag[profiler::TAG_LENGTH];
	LARGE_INTEGER _start;
	__int64 _sum;
	__int64 _max;
	__int64 _min;
	unsigned int _callCnt;

};