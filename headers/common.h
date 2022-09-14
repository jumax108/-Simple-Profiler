#pragma once

namespace nsProfiler{

	constexpr int MAX_PROFILE_NUM = 50;
	constexpr int TAG_LENGTH = 50;

	enum enErrorCode {

		SUCCESS = 0, // ����
		TOO_LONG_TAG, // �±� �̸��� �ʹ� ��
		TOO_MANY_PROFILE, // profile �迭�� ���ڶ�
		NO_PROFILE_DATA // profile �����Ͱ� ����

	};

};