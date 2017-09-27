#ifndef _UDP_H
#define _UDP_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

void bind_udp_port(int tcpfd, int udpfd);	//绑定udp port
void udp_read(int udpfd);			//读udp数据
void create_chat(int udpfd);			//建立udp聊天

#endif
