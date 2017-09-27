#include "menu.h"



#define GETCH getchar(); getchar


//void faxiaoxi(int fd);



/*
*函数名：display_menu
*功能：显示菜单
*入参:fd，
*返回值:无
*其他：无
*/
void display_menu(int* fd)
{
	char a;	
	int ret = 0;
	int udpfd = fd[1];
	int tcpfd = fd[0];
	while(1)
	{
		printf("\n");
//		system("clear");
		printf("\t\t\t欢迎使用OICQ\n\n");
		printf("\t\t\t  1-登陆\n");
		printf("\t\t\t  2-注册\n");
		printf("\t\t\t  3-退出\n\n\n");
		printf("请输入选择:");
		a = GETCH();
	
		switch(a)
		{
			case '1':
				//登陆函数
				ret = user_login(tcpfd, udpfd);
				if(ret == 1)
				{
					success_login(udpfd);
				}
				break;
			case '2':
				//注册函数
				apply_register(tcpfd);	
				break;
			case '3':
				//退出
				exit(0);
				break;
			default :
				printf("输入有误!请重新输入\n");
				break;
		}
	//	ret = read_data(tcpfd);
		
	}
}






/*
*函数名：success_login
*功能：登陆成功界面
*入参：udpfd
*返回值：int
*其他：无
*/
int success_login(int udpfd)
{
	int tag;
	
	
	
	
	while(1)
	{
		printf("\n");
//		system("clear");
		printf("\t\t\t欢迎使用OICQ\n\n");
		printf("\t\t\t1-查看好友列表\n");
		printf("\t\t\t2-开始与好友聊天\n");
//		printf("\t\t\t3-添加好友\n");
		printf("\t\t\t3-注销账户\n\n\n");
		tag = GETCH();

		switch(tag)
		{
			case '1':
				//打印好友
				print_friends();
				break;
			case '2':
				//
				print_friends();
				create_chat(udpfd);		//建立udp聊天
				//
				break;
			case '3':
				//
				exit(0);
				break;
			default :
				printf("输入选项有误\n");
		//		printf("按任意键重新输入\n");
		//		GETCH();
		}
	
	}
}



/*
*函数名：print_friends
*功能：打印好友链表
*入参：无
*返回值：无
*其他：无
*/
void print_friends()
{
	struct friends* ft = frhead;

	if(ft == NULL)
	{
		printf("没有在线好友\n");
		return;
	}
	
	printf("在线好友：\n");
	while(ft != NULL)
	{
		printf("帐号：%s\n", ft->account);
		ft = ft->next;
	}
	
}













//调试用
/*
void faxiaoxi(int fd)
{
	int ret = 0;
	int len;
	char buf[1024];
	struct sockaddr_in dfaddr = {0};
	
	memset(&dfaddr, 0, sizeof(dfaddr));
	dfaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &dfaddr.sin_addr.s_addr);
	dfaddr.sin_port = htons(20000);
	
	
	
	fgets(buf, sizeof(buf) - 1, stdin);
		len = sizeof(dfaddr);
		ret = sendto(fd, buf, strlen(buf) + 1 ,0, (struct sockaddr*)&dfaddr, len);
		printf("buf sendto\n");
}
*/
