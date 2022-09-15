#pragma once

#include <stdio.h>
#include <Windows.h>
#include <new>

#include "common.h"

class CProfiler
{
public:
	struct stProfile;

public:
	
	CProfiler();

	int begin(const char* tag);
	int end(const char* tag);

	void printToFile();

	void reset();

private:
	friend class CProfilerTLS;
	stProfile begin();
	stProfile next();
	
private:

	stProfile* _profile;
	unsigned int _allocIndex = 0;

	LARGE_INTEGER _freq;

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

	char _tag[nsProfiler::TAG_LENGTH];
	LARGE_INTEGER _start;
	__int64 _sum;
	__int64 _max;
	__int64 _min;
	unsigned int _callCnt;

};