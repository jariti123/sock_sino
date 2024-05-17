all:server client servercpp clientcpp

server:server.c
	gcc server.c -o server -g

servercpp: servercpp.cpp
	g++ servercpp.cpp -o servercpp

client:client.c
	gcc client.c -o client

clientcpp: clientcpp.cpp
	g++ clientcpp.cpp -o clientcpp

clean:
	rm -f server client servercpp clientcpp