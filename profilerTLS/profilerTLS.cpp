#include "headers/profilerTLS.h"

CProfilerTLS::CProfilerTLS() {

	_tlsIndex = TlsAlloc();
	if (_tlsIndex == TLS_OUT_OF_INDEXES) {
		CDump::crash();
	}

	_profilerAllocIndex = 0;

	//////////////////////////////////////////////////////////////////////////////////
	// 측정 단위는 100ns
	QueryPerformanceFrequency(&_freq);
	_freq.QuadPart /= 10000000;
	//////////////////////////////////////////////////////////////////////////////////

}

void CProfilerTLS::begin(const char* tag) {

	CProfiler* profiler = (CProfiler*)TlsGetValue(_tlsIndex);
	if (profiler == nullptr) {
		int profilerAllocIndex = InterlockedIncrement((LONG*)&_profilerAllocIndex) - 1;

		if (profilerAllocIndex >= profilerTLS::THREAD_NUM) {
			CDump::crash();
		}

		profiler = &_profiler[profilerAllocIndex];
		TlsSetValue(_tlsIndex, profiler);
	}

	profiler->begin(tag);

}

void CProfilerTLS::end(const char* tag) {

	CProfiler* profiler = (CProfiler*)TlsGetValue(_tlsIndex);
	profiler->end(tag);

}

void CProfilerTLS::printToFile() {

	CProfiler* profilerIter = _profiler;
	CProfiler* profilerEnd = _profilerAllocIndex + _profiler;

	FILE* outFile;
	fopen_s(&outFile, "profilerTLS.txt", "w");

	fprintf_s(outFile, "%15s | %20s | %15s | %15s | %15s | %15s \n", "Thread ID", "Name", "Average", "Min", "Max", "Call");

	for (; profilerIter != profilerEnd; ++profilerIter) {

		CProfiler::stProfile* profileData = profilerIter->begin();

		do {

			profileData->_sum = 
				profileData->_sum - profileData->_max - profileData->_min;
			profileData->_callCnt -= 2;

			fprintf_s(outFile, "%15d | %20s | %12.3lf us | %12.3lf us | %12.3lf us | %15d \n",
				profileData->_threadId,
				profileData->_name,
				profileData->_sum / (double)_freq.QuadPart / profileData->_callCnt,
				profileData->_min / (double)_freq.QuadPart,
				profileData->_max / (double)_freq.QuadPart,
				profileData->_callCnt);

			profileData = profilerIter->next();

		} while (profileData != nullptr);

	}

}

void CProfilerTLS::reset() {

	for (int profilerCnt = 0; profilerCnt < profilerTLS::THREAD_NUM; ++profilerCnt) {
		_profiler[profilerCnt].reset();
	}

}