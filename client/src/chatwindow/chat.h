#ifndef _CHAT_H
#define _CHAT_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>



struct forkdata
{
	char tag;
	char myname[10];
	char dfname[10];
	struct sockaddr_in dfaddr;
};


#endif
