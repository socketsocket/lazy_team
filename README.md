- 코드 컨벤션
	- 최대한 노미넷을 위배하는 방향으로
[구글 스타일](https://google.github.io/styleguide/cppguide.html#The__define_Guard)

- 괄호 위치는 첫줄에 포함
- return에는 최대한 괄호가 없게 함

- The #define Guard
	- 최대한 노미넷을 위배하는 방향으로
	- #와 define 사이는 띄지 않기
	- <PROJECT>_<PATH>_<FILE>_H_. 방식

- webserv헤더는 스탠다드 라이브러리가 아닌 아무것도 include 하지않고, 다만 매크로나 enum 등을 define함.

- kqueue event (소켓 프로그래밍) : jolim
- read/write event : jinbekim
- response/resource : seohchoi
- cgi :
