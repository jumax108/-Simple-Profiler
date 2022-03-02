#include "headers/profiler.h"

void CProfiler::begin(const char name[100]) {
	
	////////////////////////////////////////////////
	// ���� profile data ȹ��
	Profile* profile = getTlsProfileData();
	int idx = findIdx(name, profile);
	////////////////////////////////////////////////

	////////////////////////////////////////////////
	// ���� �����Ͱ� ������ ����
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
	// ���� �����Ͱ� ������ count ���
	else {
		profile[idx].callCnt += 1;
	}
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	// ���� ����
	QueryPerformanceCounter(&profile[idx].start);
	////////////////////////////////////////////////
}

void CProfiler::end(const char name[100]) {
		
	////////////////////////////////////////////////
	// ���� ����
	LARGE_INTEGER endTime;
	QueryPerformanceCounter(&endTime);
	////////////////////////////////////////////////

	////////////////////////////////////////////////
	// ���� profile data ȹ��
	Profile* profile = getTlsProfileData();
	int idx = findIdx(name, profile);
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	// ���� �ÿ��� ���� ������ ������ ����
	if(idx == -1){
		CDump::crash();
		return ;
	}
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	// ��ü �ð��� ��� �ð� �ջ�
	profile[idx].sum += endTime.QuadPart - profile[idx].start.QuadPart;
	////////////////////////////////////////////////
}

CProfiler::Profile* CProfiler::getTlsProfileData(){
	
	////////////////////////////////////////////////
	// ���� ������ ȹ��
	Profile* ptr = (Profile*)TlsGetValue(tlsIndex);
	////////////////////////////////////////////////

	
	////////////////////////////////////////////////
	// ���� �����Ͱ� ���ٸ� ���� �� ���
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
	// �̸��� ���� �׸� ã�Ƽ� �ε��� ����
	int idx = 0;
	for(; idx < 50 ; ++idx){		
		if(strcmp(name, profile[idx].name) != 0){
			continue;
		}
		return idx;
	}
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	// ��ã���� -1 ����
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
	// tls �Ҵ�
	tlsIndex = TlsAlloc();
	if(tlsIndex == TLS_OUT_OF_INDEXES){
		// tls �Ҵ� ����
		logger(L"profiler.txt", LOG_GROUP::LOG_ERROR, L"tls alloc error\n%s\n%d\n", __FILEW__, __LINE__);
		CDump::crash();
	}
	//////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////
	// ���� ������ 100ns
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
	