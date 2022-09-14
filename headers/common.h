#pragma once

namespace nsProfiler{

	constexpr int MAX_PROFILE_NUM = 50;
	constexpr int TAG_LENGTH = 50;

	enum enErrorCode {

		SUCCESS = 0, // 성공
		TOO_LONG_TAG, // 태그 이름이 너무 김
		TOO_MANY_PROFILE, // profile 배열이 모자람
		NO_PROFILE_DATA // profile 데이터가 없음

	};

};