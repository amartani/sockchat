# vim: noet ts=8 sts=8 sw=8

CC	= gcc
CFLAGS	= -g

default: server client coordinator
testserver: server
	py.test test_server.py
testclient: client
	ruby test_client.rb
client: client.c
	$(CC) $(CFLAGS) client.c -o client -pthread -lm
server: server.c
	$(CC) $(CFLAGS) server.c -o server -pthread -lm
coordinator: coordinator.c
	$(CC) $(CFLAGS) coordinator.c -o coordinator -pthread -lm
clean:
	rm -f client server coordinator
