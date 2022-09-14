
# Profiler
싱글 스레드용 성능 측정 도구

## 사전 세팅

- common.h
	- nsProfiler::MAX_PROFILER_NUM 
		> 프로파일링 가능한 태그의 최대 수
	- nsProfiler::TAG_LENGTH
		> 태그 이름의 최대 길이

## 함수
 - int begin(const char* tag)
	 > 입력된 tag에 대하여 성능 측정 시작 <br>
	> 성공 시 0 리턴, 오류 시 에러코드 리턴
 - int end(const char* tag)
	 > 입력된 tag에 대하여 성능 측정 종료
	> 성공 시 0 리턴, 오류 시 에러코드 리턴
 - void printToFile()
	 > 현재까지 수집된 성능 자료를 파일로 출력 <br>
	 > profiler.txt 에 저장됨
 - void reset()
	 > 현재까지 수집된 성능 자료를 초기화

## 에러 코드
```cpp
enum nsProfiler::enErrorCode {

 SUCCESS = 0,
 TOO_LONG_TAG,
 TOO_MANY_PROFILE,
 NO_PROFILE_DATA

};
```

## 사용법
```cpp

CDump dump;

void main(){

  CProfiler p;

  for(int i=0 ; i<10 ; i++){
    p.begin("tag")
    // 측정 대상
    p.end("tag")
  }

  p.printToFile();

}
```