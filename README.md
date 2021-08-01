### 구조
1. ServerFd ReadEvent 발생 -> Client 생성
2. ClientFd ReadEvent 발생 -> RawRequest 생성하여 Client에게 넘김 -> Client가 RawRequest 파싱하여 Re3 Deque에 push Re3 -> Re3_iter를 PortManager 통하여 Server에 넘김 -> Server는 vector(resourceFd)로 Re3_iter 관리 -> resource 필요하다면 해당 Re3에 resource 등록, return. -> Client는 return하기 전에 새로 만든 Re3들을 살펴보고 read 필요한 Resource 있다면 vector에 넣어 return. -> ServerManager는 return 받은 vector의 Re3를 살펴서 등록이 필요한 것이 있다면 kq에 등록
3. ResourceFd ReadEvent 발생 -> Resource 자체 메소드에서 Read -> read 끝나지 않은 경우 내버려두고, 끝난 경우 Server 호출. Server는 가지고 있는 vector에서 해당 Resource 찾아서 Response 완료. Client 호출할 필요 없음
4. Client WriteEvent 발생 -> Client는 가지고 있는 Re3 Queue를 확인하여 가장 앞의 Response가 완성되어있는지 확인. -> 된 경우 pop(delete)하면서 본문을 버퍼 크기까지 이어붙임. -> write.
5. Stderr WriteEvent 발생 -> ErrQueue에서 버퍼크기만큼 이어붙여서 stderr에 출력.

### 코드 컨벤션
- 최대한 노미넷을 위배하는 방향으로

[구글 스타일](https://google.github.io/styleguide/cppguide.html#The__define_Guard)

- 가로줄 길이는 웬만하면 80자를 넘지 않게 함, 120자 이상 절대 넘지 않게 함
- 줄바꿈 시 오퍼레이터가 사이에 있는 경우, 오퍼레이터가 개행에 포함되도록

- 중괄호 위치는 첫줄에 포함, 함수인경우 띄어쓰기 X, 아닌경우 띄어쓰기 O
- return에는 최대한 괄호가 없게 함

- 멤버 변수를 부를 때는 가능한 한 반드시 this를 붙이자
- 멤버 메소드를 부를 때도 가능한 한 반드시 this를 붙이자
- 스태틱 멤버변수를 해당 클래스 내에서 사용할 때, 반드시 클래스 명을 명시할 것

- The #define Guard
	- 최대한 노미넷을 위배하는 방향으로
	- #와 define 사이는 띄지 않기
	- `<PROJECT>_<PATH>_<FILE>_H_`. 방식

- webserv헤더는 스탠다드 라이브러리가 아닌 아무것도 include 하지않고, 다만 매크로나 enum 등을 define함.

- kqueue event (소켓 프로그래밍) : jolim
- read/write event : jinbekim
- response/resource : seohchoi
- ERROR 규칙
```
#define ERROR -1
#define OK     0
```

```
std::cerr << "ConfigParser: " << "getIntoBlock: " << NAME_MATCH_ERR << std::endl;
```

---

### 역할분담

- kqueue event (소켓 프로그래밍) / config파일 파싱 : jolim
	- ServerManager
	- ConfigParser
		- Location 생성
		- Server 생성
		- config파일의 path는 반드시 /로 끝날 것
	- ErrorMsgHandler
	- Re3


- read/write event/request 파싱 : jinbekim
	- Client
		- Request를 읽고 파싱
		- Request / Response -> 요청 리퀘스트/리스폰스 스트링을 '담아두는' 클래스
	- PortManager
		- 클라이언트의 Request를 받아 올바른 Server로 데이터를 넘겨주는 클래스


- response 클래스 제작/response 처리/resource : seohchoi
	- Server
		- 파싱된 Request 받아 까보기
		- Request에 따라 Response 제작 / Response가 필요하다면 Response open
		- Response가 완성되면 Client->Response에 입력
	- Response
		- 완성된 응답저장 및 반환
	- Resource
		- 리소스 read하고 저장
		- 리소스를 다 읽었는지/아닌지 상태 저장

- cgi : 이것은 우리 모두의 책임입니다.
