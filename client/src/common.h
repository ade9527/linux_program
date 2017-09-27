#ifndef _COMMON_H
#define _COMMON_H


#include <netinet/in.h>



#define SER_PORT 8080

#define LOGIN_DATA    1000
#define REGISTER_DATA 1001
#define ONLINE_DATA   1002
#define OFFLINE_DATA  1003
#define PANT_DATA     1004


struct protocol				//协议结构体
{
	short head;
	short len;
	char data[1020];
};


struct friends				//在线好友信息
{
	char account[10];
	struct sockaddr_in dfaddr;	//ip port
	struct friends* next;
};

struct forkdata
{
	char tag;
	char myname[10];
	char dfname[10];
	struct sockaddr_in dfaddr;
};



extern struct friends* frhead;
extern char myaccount[];



#endif
