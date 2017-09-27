#ifndef _TCPTREAT_H
#define _TCPTREAT_H

#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>



int read_data(int tcpfd);			//读取tcp数据
void add_online(struct protocol *datum);	//增加在线链表
void del_online(struct protocol *datum);	//删除下线节点
int user_login(int tcpfd, int udpfd);		//发送登陆信息 函数
int apply_register(int tcpfd);			//发送注册信息函数

void send_pant(int tcpfd);			//发送心跳包每 5s

#endif
