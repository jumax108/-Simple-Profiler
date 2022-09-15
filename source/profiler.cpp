#include "..\headers\profiler.h"

int CProfiler::begin(const char* tag) {
	
	/* reset */ {
		if (_reset == true) {
			_reset = false;
			for (int dataCnt = 0; dataCnt < nsProfiler::MAX_PROFILE_NUM; ++dataCnt) {
				_profile[dataCnt]._sum = 0;
				_profile[dataCnt]._max = 0;
				_profile[dataCnt]._min = 0x7FFFFFFFFFFFFFFF;
				_profile[dataCnt]._callCnt = 0;
			}
		}
	}

	/* �±װ� �ʹ� �� */
	if (strlen(tag) > nsProfiler::TAG_LENGTH) {
		return nsProfiler::enErrorCode::TOO_LONG_TAG;
	}

	/* ���� ������ Ȯ�� */
	int idx = findIdx(tag);

	////////////////////////////////////////////////
	// ���� �����Ͱ� ������ ����
	if(idx == -1){

		idx = _allocIndex++;
		if (idx >= nsProfiler::MAX_PROFILE_NUM) {
			return nsProfiler::enErrorCode::TOO_MANY_PROFILE;
		}

		_profile[idx]._callCnt = 1;
		strcpy_s(_profile[idx]._tag, tag);

	}
	////////////////////////////////////////////////
		
	////////////////////////////////////////////////
	// ���� �����Ͱ� ������ count ���
	else {
		_profile[idx]._callCnt += 1;
	}
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	// ���� ����
	QueryPerformanceCounter(&_profile[idx]._start);
	////////////////////////////////////////////////

	return nsProfiler::enErrorCode::SUCCESS;
}

int CProfiler::end(const char* tag) {
		
	////////////////////////////////////////////////
	// ���� ����
	LARGE_INTEGER endTime;
	QueryPerformanceCounter(&endTime);
	////////////////////////////////////////////////

	/* �±װ� �ʹ� �� */
	if (strlen(tag) > nsProfiler::TAG_LENGTH) {
		return nsProfiler::enErrorCode::TOO_LONG_TAG;
	}

	////////////////////////////////////////////////
	// ���� profile data ȹ��
	int idx = findIdx(tag);
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	// ���� �ÿ��� ���� ������ ������ ����
	if(idx == -1){
		return nsProfiler::enErrorCode::NO_PROFILE_DATA;
	}
	////////////////////////////////////////////////
	
	stProfile* profileData = &_profile[idx];

	////////////////////////////////////////////////
	// ��� �ð� ���
	long long spendTime = endTime.QuadPart - profileData->_start.QuadPart;
	////////////////////////////////////////////////

	////////////////////////////////////////////////
	// ��� �ݿ�
	profileData->_sum += spendTime;
	if(profileData->_max < spendTime){
		profileData->_max = spendTime;
	}
	if(profileData->_min > spendTime){
		profileData->_min = spendTime;
	}
	////////////////////////////////////////////////

	return nsProfiler::enErrorCode::SUCCESS;
}

int CProfiler::findIdx(const char* name) {
		
	////////////////////////////////////////////////
	// �̸��� ���� �׸� ã�Ƽ� �ε��� ����
	int idx = 0;
	for(; idx < nsProfiler::MAX_PROFILE_NUM; ++idx){
		if(strcmp(name, _profile[idx]._tag) != 0){
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

	/* profile �迭 �Ҵ� */ {
		_profile = (stProfile*)malloc(sizeof(stProfile) * nsProfiler::MAX_PROFILE_NUM);
					
		stProfile* profileIter = _profile;
		stProfile* profileEnd = profileIter + nsProfiler::MAX_PROFILE_NUM;

		for (; profileIter != profileEnd; ++profileIter) {
			new (profileIter) stProfile();
		}
	}

	/* data �ʱ�ȭ */ {
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

		/* ���µǾ� �����Ͱ� ������ break */
		if (_reset == true) {
			break;
		}

		for (int idx = 0; idx < nsProfiler::MAX_PROFILE_NUM; ++idx) {

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

CProfiler::stProfile CProfiler::begin() {

	_returnIndex = 0;
	return _profile[_returnIndex];

}

CProfiler::stProfile CProfiler::next() {

	_returnIndex += 1;
	
	if (_returnIndex == _allocIndex) {
		stProfile temp;
		temp._tag[0] = '\0';
		return temp;
	}

	return _profile[_returnIndex];

}

void CProfiler::reset() {

	_reset = true;

	// reset ������ ��û���ϰ� ���� reset �۾��� begin���� �����Ѵ�.
	// ��Ƽ������ �������� ������ ��, reset �۾��� begin �۾��� ���ÿ� ����Ǵ� ��Ȳ�� ���ֱ� ����

}