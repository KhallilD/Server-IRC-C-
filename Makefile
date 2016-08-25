
FLAGS= -W -Wall -Wextra

all: Server

re : clean all

Server: main.cpp Server.cpp Channel.cpp User.cpp
	g++ -o Server main.cpp Server.cpp Channel.cpp User.cpp $(FLAGS)

clean:
	rm -rf Server
	rm -rf *~

fclean: clean
