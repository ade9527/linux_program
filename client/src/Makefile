client:client.o menu.o tcptreat.o udp.o
	gcc client.o menu.o tcptreat.o udp.o -o client -lpthread -g
client.o:client.c
	gcc client.c -c -g
menu.o:menu.c
	gcc menu.c -c -g
tcptreat.o:tcptreat.c
	gcc tcptreat.c -c -g
udp.o:udp.c
	gcc udp.c -c -g

clean:
	rm *.o client
