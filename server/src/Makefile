server:server.o treat.o
	gcc server.o treat.o -o server -pthread -g
server.o:server.c
	gcc server.c -c -g
treat.o:treat.c
	gcc treat.c -c -g
clean:
	rm *.o server
