# Webserv

# *Summary*:
This project is here to make you write your HTTP server.

# *Period*:
2021.08. ~ 2021.10

# *CMD*:
> `$> make all`
>
> `$> make debug && ./webserv`


## *Structure*:
<img src="https://user-images.githubusercontent.com/59330110/137316671-2ffe88a7-d14c-498b-b27b-1d199e906bb6.png" alt="drawing" width="500"/>

1. ServerFd ReadEvent 발생 -> `ClientFd 생성`
2. ClientFd ReadEvent 발생 -> `Client에서 Request 파싱`
3. Request(Clinet) -> Server
   1. Read 필요한 `Resource Fd Read Event 등록`, Server(ResourceFd) -> Client -> ServerManager
   2. ResourceFd ReadEvent 발생 -> `Resource를 Server로` 전달
4. `Server에서 Response 생성`
5. Client WriteEvent 발생 -> Client는 Response가 완성되어있는지 확인. -> `send`
6. Stderr WriteEvent 발생 -> stderr에 출력.

## *Code Convention*:
> [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html#The__define_Guard)

## *Reference*:
> https://developer.mozilla.org/ko/docs/Web/HTTP
>

## *Project Contribution*:

- *jolim*
	- kqueue event (소켓 프로그래밍) / config파일 파싱
	- ServerManager
	- ConfigParser
		- Location 생성
		- Server 생성
	- TermPrinter
	- Re3
		- Request
		- Response
		- Resource

- *seohchoi*
	- response 클래스 제작
		- 완성된 응답저장 및 반환
	- Server
		- 파싱된 Request 받아 까보기
		- Request에 따라 Response 제작 / Response가 필요하다면 Response open
		- Response가 완성되면 Client->Response에 입력
	- Resource
		- 리소스 read하고 저장
		- 리소스를 다 읽었는지/아닌지 상태 저장

- *jinbekim*
	- read/write event/request 파싱
	- Client
		- Request를 읽고 파싱
		- Request / Response -> 요청 리퀘스트/리스폰스 스트링을 '담아두는' 클래스
	- PortManager
		- 클라이언트의 Request를 받아 올바른 Server로 데이터를 넘겨주는 클래스
	- CGI
