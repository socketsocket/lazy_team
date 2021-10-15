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

## *Goal*:
- [x] GET, POST, DELETE
- [x] multiple ports
- [x] CGI
- [x] multiplexing

## *Code Convention*:
> [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html#The__define_Guard)

## *Reference*:
> [HTTP](https://developer.mozilla.org/ko/docs/Web/HTTP)
> [Poller](http://openlook.org/src/articles/maso0109-kqueue.pdf)

## *Project Contribution*:

- *jolim*
	- ServerManager
		- poller: kqueue
		- multiplexing
	- ConfigParser
		- config parsing
	- TermPrinter

- *seohchoi*
	- Server
		- Request -> Response
		- Resource

- *jinbekim*
	- Client
		- Request parsing
	- CgiConnector
		- CGI
