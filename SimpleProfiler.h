#pragma once
class SimpleProfiler
{
public:
	struct Profile {
		bool use = false;
		char name[100] = {0,};
		LARGE_INTEGER start;
		__int64 sum = 0;
		__int64 max = 0;
		__int64 min = 0x7FFFFFFFFFFFFFFF;
		unsigned int callCnt = 0;
	};

	LARGE_INTEGER freq;

public:

	SimpleProfiler() {
		QueryPerformanceFrequency(&freq);
	}
	
	void profileBegin(const char name[100]) {
		int idx = findIdx(name);
		if (idx == -1) {
			idx = getEmptyIdx();
			if (idx == -1) {
				killProcess();
			}
		}

		info[idx].callCnt += 1;
		info[idx].use = true;
		strcpy_s(info[idx].name, name);	

		QueryPerformanceCounter(&info[idx].start);
	}

	void profileEnd(const char name[100]) {
		int idx = findIdx(name);
		if (idx == -1) {
			killProcess();
		}

		LARGE_INTEGER endTime;
		QueryPerformanceCounter(&endTime);

		__int64 time = endTime.QuadPart - info[idx].start.QuadPart;

		if (info[idx].max < time) {
			info[idx].max = time;
		}

		if (info[idx].min > time) {
			info[idx].min = time;
		}

		info[idx].sum += time;

		info[idx].use = false;

	}

	void printResult() {
		for (int infoCnt = 0; infoCnt < 50; infoCnt++) {
			if (info[infoCnt].callCnt > 0) {
				info[infoCnt].sum = info[infoCnt].sum - info[infoCnt].max - info[infoCnt].min;
				info[infoCnt].callCnt -= 2;
				printf_s("%s %lf %lf %d\n", info[infoCnt].name, info[infoCnt].sum / (double)freq.QuadPart, info[infoCnt].sum / (double)freq.QuadPart / info[infoCnt].callCnt, info[infoCnt].callCnt);
			}
		}
	}

private:
	Profile info[50];

	int findIdx(const char* name) {

		for (int infoCnt = 0; infoCnt < 50; ++infoCnt) {
			if (strcmp(name, info[infoCnt].name) == 0) {
				return infoCnt;
			}
		}

		return -1;
	}

	int getEmptyIdx() {
		for (int infoCnt = 0; infoCnt < 50; ++infoCnt) {
			if (info[infoCnt].use == 0 && info[infoCnt].callCnt == 0) {
				return infoCnt;
			}
		}

		return -1;
	}

	void killProcess() {
		int* kill = nullptr;
		*kill = 1;
	}


};

