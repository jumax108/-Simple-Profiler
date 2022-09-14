# Profiler
싱글 스레드용 성능 측정 도구

## 사전 세팅

- common.h
	- profiler::MAX_PROFILER_NUM 
		> 프로파일링 가능한 태그의 최대 수
	- profiler::TAG_LENGTH
		> 태그 이름의 최대 길이
	- Dump를 위해 CDump 객체를 만든다.
	  > CDump dump; <br>
	  > 생성자에 초기화 코드가 있어, 생성자를 호출시키면 된다. <br>
	  > 실제 사용을 위한 객체는 아니다.

## 함수
 - void begin(const char* tag)
	 > 입력된 tag에 대하여 성능 측정 시작
 - void end(const char* tag)
	 > 입력된 tag에 대하여 성능 측정 종료
 - void printToFile()
	 > 현재까지 수집된 성능 자료를 파일로 출력 <br>
	 > profiler.txt 에 저장됨
 - void reset()
	 > 현재까지 수집된 성능 자료를 초기화

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
