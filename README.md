### 코드 컨벤션
- 최대한 노미넷을 위배하는 방향으로

[구글 스타일](https://google.github.io/styleguide/cppguide.html#The__define_Guard)

- 가로줄 길이는 웬만하면 80자를 넘지 않게 함, 120자 이상 절대 넘지 않게 함
- 줄바꿈 시 오퍼레이터가 사이에 있는 경우, 오퍼레이터가 개행에 포함되도록

- 중괄호 위치는 첫줄에 포함, 함수인경우 띄어쓰기 X, 아닌경우 띄어쓰기 O
- return에는 최대한 괄호가 없게 함

- 멤버 변수를 부를 때는 가능한 한 반드시 this를 붙이자
- 멤버 메소드를 부를 때도 가능한 한 반드시 this를 붙이자

- The #define Guard
	- 최대한 노미넷을 위배하는 방향으로
	- #와 define 사이는 띄지 않기
	- *<PROJECT>_<PATH>_<FILE>_H_.* 방식

- webserv헤더는 스탠다드 라이브러리가 아닌 아무것도 include 하지않고, 다만 매크로나 enum 등을 define함.
- 스태틱 멤버변수를 해당 클래스 내에서 사용할 때, 반드시 클래스 명을 명시할 것

---

### 역할분담

- kqueue event (소켓 프로그래밍) / config파일 파싱 : jolim
	- ServerManager
	- ConfigParser
		- Location 생성
		- Server 생성
	- Location 
	- Server

- read/write event/request 파싱 : jinbekim
	- Client
		- Request를 읽고 파싱
		- Response를 쓰기 
		- Request / Response -> 요청 리퀘스트/리스폰스 스트링을 '담아두는' 클래스

- response 클래스 제작/response 처리/resource : seohchoi
	- Server
		- Response 제작 및 Client->Response에 입력
	- Response
	- Resource
	
- cgi :
