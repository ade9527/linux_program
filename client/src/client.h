#ifndef _CLIENT_H
#define _CLIENT_H

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>


#include "common.h"
#include "menu.h"
#include "tcptreat.h"
#include "udp.h"



/*

struct friends* frhead = NULL;

int login_tag = 0;
int register_tag = 0;

char myaccount[10];

*/


struct friends* frhead = NULL;
char myaccount[10];



int client();					//建立套接字，监听事件发生，
static void sig_routine(int signo);		//接收子进程退出信号


#endif
