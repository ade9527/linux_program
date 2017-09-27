#ifndef _TREAT_H
#define _TREAT_H


#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>




#define LOGIN_DATA    1000
#define REGISTER_DATA 1001
#define ONLINE_DATA   1002
#define OFFLINE_DATA  1003 
#define PANT_DATA     1004

#define TIME_PANT 4

struct protocol				//协议结构体
{
	short head;
	short len;
	char data[1020];
};


struct account
{
	char username[10];
	char password[10];
};


struct online				//在线用户
{
	int accfd;	
	int time;
	char account[10];
	struct sockaddr_in accaddr;	//ip port	
	struct online* next;
};


extern struct online* olhead;
extern struct online* linkhead;



int read_data(int fd);				//处理接收的数据
void login_treat(int fd, struct protocol* datum);	//处理登陆数据
void treat_register(int fd, struct protocol* datum);	//处理注册信息
void treat_pant(int fd);			//处理接收到的心跳数据

void send_offline(struct online* offline);		//发送下线通知
void add_online(int fd, struct protocol* datum);	//添加上线节点
void print_online();				//打印在线信息
	

#endif
