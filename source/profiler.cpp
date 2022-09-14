#include "..\headers\profiler.h"

void CProfiler::begin(const char name[100]) {
	
	/* reset */ {
		if (_reset == true) {
			_reset = false;
			for (int dataCnt = 0; dataCnt < profiler::MAX_PROFILE_NUM; ++dataCnt) {
				_profile[dataCnt]._sum = 0;
				_profile[dataCnt]._max = 0;
				_profile[dataCnt]._min = 0x7FFFFFFFFFFFFFFF;
				_profile[dataCnt]._callCnt = 0;
			}
		}
	}

	/* 기존 데이터 확인 */
	int idx = findIdx(name);

	////////////////////////////////////////////////
	// 기존 데이터가 없으면 생성
	if(idx == -1){

		idx = _allocIndex++;
		if (idx >= profiler::MAX_PROFILE_NUM) {
			CDump::crash();
		}

		_profile[idx]._callCnt = 1;
		strcpy_s(_profile[idx]._tag, name);

	}
	////////////////////////////////////////////////
		
	////////////////////////////////////////////////
	// 기존 데이터가 있으면 count 상승
	else {
		_profile[idx]._callCnt += 1;
	}
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	// 측정 시작
	QueryPerformanceCounter(&_profile[idx]._start);
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
	int idx = findIdx(name);
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	// 종료 시에는 기존 데이터 없으면 에러
	if(idx == -1){
		CDump::crash();
		return ;
	}
	////////////////////////////////////////////////
	
	stProfile* profileData = &_profile[idx];

	////////////////////////////////////////////////
	// 경과 시간 계산
	long long spendTime = endTime.QuadPart - profileData->_start.QuadPart;
	////////////////////////////////////////////////

	////////////////////////////////////////////////
	// 결과 반영
	profileData->_sum += spendTime;
	if(profileData->_max < spendTime){
		profileData->_max = spendTime;
	}
	if(profileData->_min > spendTime){
		profileData->_min = spendTime;
	}
	////////////////////////////////////////////////
}

int CProfiler::findIdx(const char* name) {
		
	////////////////////////////////////////////////
	// 이름이 같은 항목 찾아서 인덱스 리턴
	int idx = 0;
	for(; idx < profiler::MAX_PROFILE_NUM; ++idx){
		if(strcmp(name, _profile[idx]._tag) != 0){
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
	_logger.setDirectory(L"log");
	_logger.setPrintGroup(LOG_GROUP::LOG_ERROR | LOG_GROUP::LOG_SYSTEM);
	//////////////////////////////////////////////////////////////////////////////////

	/* profile 배열 할당 */ {
		_profile = (stProfile*)malloc(sizeof(stProfile) * profiler::MAX_PROFILE_NUM);
					
		stProfile* profileIter = _profile;
		stProfile* profileEnd = profileIter + profiler::MAX_PROFILE_NUM;

		for (; profileIter != profileEnd; ++profileIter) {
			new (profileIter) stProfile();
		}
	}

	/* data 초기화 */ {
		_returnIndex = 0;
		_reset = false;
	}
}

void CProfiler::printToFile() {

	FILE* outFile;
	fopen_s(&outFile, "profiler.txt", "w");

	fprintf_s(outFile, "(100ns)\n");
	fprintf_s(outFile, "%20s | %15s | %15s | %15s | %15s \n", "Name", "Average", "Min", "Max", "Call");
		
	do {

		/* 리셋되어 데이터가 없으면 break */
		if (_reset == true) {
			break;
		}

		for (int idx = 0; idx < profiler::MAX_PROFILE_NUM; ++idx) {

			if (_profile[idx]._callCnt > 2) {
				_profile[idx]._sum = _profile[idx]._sum - _profile[idx]._max - _profile[idx]._min;
				_profile[idx]._callCnt -= 2;
				fprintf_s(outFile, "%20s | %12.3lf us | %12.3lf us | %12.3lf us | %15d \n",
					_profile[idx]._tag,
					_profile[idx]._sum / (double)_freq.QuadPart / _profile[idx]._callCnt,
					_profile[idx]._min / (double)_freq.QuadPart,
					_profile[idx]._max / (double)_freq.QuadPart,
					_profile[idx]._callCnt);
			}
		}

	} while (false);

	fclose(outFile);

}

CProfiler::stProfile* CProfiler::begin() {

	_returnIndex = 0;
	return &_profile[_returnIndex];

}

CProfiler::stProfile* CProfiler::next() {

	_returnIndex += 1;
	
	if (_returnIndex == _allocIndex) {
		return nullptr;
	}

	return &_profile[_returnIndex];

}

void CProfiler::reset() {

	_reset = true;

	// reset 동작을 요청만하고 실제 reset 작업은 begin에서 진행한다.
	// 멀티스레드 버전에서 동작할 때, reset 작업과 begin 작업이 동시에 진행되는 상황을 없애기 위함

}