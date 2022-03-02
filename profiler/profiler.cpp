#include "headers/profiler.h"

void CProfiler::begin(const char name[100]) {
	
	////////////////////////////////////////////////
	// 기존 profile data 획득
	Profile* profile = getTlsProfileData();
	int idx = findIdx(name, profile);
	////////////////////////////////////////////////

	////////////////////////////////////////////////
	// 기존 데이터가 없으면 생성
	if(idx == -1){
		for(idx = 0; idx < 50; idx++){
			if(profile[idx].callCnt == 0){
				break;
			}
		}

		profile[idx].callCnt = 1;
		strcpy_s(profile[idx].name, name);
	}
	////////////////////////////////////////////////
		
	////////////////////////////////////////////////
	// 기존 데이터가 있으면 count 상승
	else {
		profile[idx].callCnt += 1;
	}
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	// 측정 시작
	QueryPerformanceCounter(&profile[idx].start);
	////////////////////////////////////////////////
}

void CProfiler::end(const char name[100]) {
		
	////////////////////////////////////////////////
	// 측정 종료
	LARGE_INTEGER endTime;
	QueryPerformanceCounter(&endTime);
	////////////////////////////////////////////////

	////////////////////////////////////////////////
	// 기존 profile data 획득
	Profile* profile = getTlsProfileData();
	int idx = findIdx(name, profile);
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	// 종료 시에는 기존 데이터 없으면 에러
	if(idx == -1){
		CDump::crash();
		return ;
	}
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	// 전체 시간에 경과 시간 합산
	profile[idx].sum += endTime.QuadPart - profile[idx].start.QuadPart;
	////////////////////////////////////////////////
}

CProfiler::Profile* CProfiler::getTlsProfileData(){
	
	////////////////////////////////////////////////
	// 기존 데이터 획득
	Profile* ptr = (Profile*)TlsGetValue(tlsIndex);
	////////////////////////////////////////////////

	
	////////////////////////////////////////////////
	// 기존 데이터가 없다면 생성 후 등록
	if(ptr == nullptr){
		int idx  = InterlockedIncrement16((SHORT*)&allocIndex) - 1;
		ptr = profile[idx];
		TlsSetValue(tlsIndex, ptr);
	}
	////////////////////////////////////////////////

	return ptr;

}

int CProfiler::findIdx(const char* name, Profile* profile) {
		
	////////////////////////////////////////////////
	// 이름이 같은 항목 찾아서 인덱스 리턴
	int idx = 0;
	for(; idx < 50 ; ++idx){		
		if(strcmp(name, profile[idx].name) != 0){
			continue;
		}
		return idx;
	}
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	// 못찾으면 -1 리턴
	return -1;
	////////////////////////////////////////////////
}

CProfiler::CProfiler(){

	//////////////////////////////////////////////////////////////////////////////////
	// logger setting
	logger.setDirectory(L"log");
	logger.setPrintGroup(LOG_GROUP::LOG_ERROR | LOG_GROUP::LOG_SYSTEM);
	//////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////
	// tls 할당
	tlsIndex = TlsAlloc();
	if(tlsIndex == TLS_OUT_OF_INDEXES){
		// tls 할당 실패
		logger(L"profiler.txt", LOG_GROUP::LOG_ERROR, L"tls alloc error\n%s\n%d\n", __FILEW__, __LINE__);
		CDump::crash();
	}
	//////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////
	// 측정 단위는 100ns
	QueryPerformanceFrequency(&freq);
	freq.QuadPart /= 10000000;
	//////////////////////////////////////////////////////////////////////////////////

}

void CProfiler::printToFile() {

	FILE* outFile;
	fopen_s(&outFile, "profiler.txt", "w");

	fprintf_s(outFile, "%20s | %15s | %15s | %15s | %15s \n", "Name", "Average", "Min", "Max", "Call");
		
	Profile* profile = getTlsProfileData();

	for (int idx = 0; idx < 50; ++idx) {
			
		if (profile[idx].callCnt > 0) {
			profile[idx].sum = profile[idx].sum - profile[idx].max - profile[idx].min;
			profile[idx].callCnt -= 2;
			fprintf_s(outFile, "%20s | %12.3lf us | %12.3lf us | %12.3lf us | %15d \n",
				profile[idx].name,
				profile[idx].sum / (double)freq.QuadPart / profile[idx].callCnt,
				profile[idx].min / (double)freq.QuadPart,
				profile[idx].max / (double)freq.QuadPart,
				profile[idx].callCnt);
		}
	}
		
	fclose(outFile);

}
	