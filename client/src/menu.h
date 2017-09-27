#ifndef _MENU_H
#define _MENU_H

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "tcptreat.h"



void display_menu(int* fd);			//显示菜单
int success_login(int udpfd);			//登陆成功界面
void print_friends();				//打印好友链表


#endif
