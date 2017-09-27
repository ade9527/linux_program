#include "common.h"
#include "udp.h"




/*
*函数名：bind_udp_port
*功能：udp绑定成tcp的port
*入参：tcpfd udpfd
*返回值：无
*其它：无
*/
void bind_udp_port(int tcpfd, int udpfd)
{
	int ret = 0;
	struct sockaddr_in myaddr = {0};
	int len = 0;
	
	// bu xu yao le 
	ret = read(tcpfd, &myaddr, sizeof(myaddr));
	if(ret == -1)
	{
		printf("read myaddr error\n");
		exit(1);
	}
	
	// get my addr
	ret = getsockname(tcpfd, &myaddr, &len);
	if(ret == -1)
	{
		printf("get my address fail\n");
		exit(1);
	}



//	printf("bind...\n");
	ret = bind(udpfd, (struct sockaddr*)&myaddr, sizeof(myaddr));
	if(ret == -1)
	{
		printf("bind error\n");
		exit(1);
	}
	return;
}



/*
*函数名：udp_read
*功能：读udp数据，开启聊天窗口
*入参：udpfd  udp套接字
*返回值：无
*其它：建立进程使用execl()
*/
void udp_read(int udpfd)
{
	struct sockaddr_in udpaddr = {0};
	struct forkdata forkbuf = {0};
	char name[10] = {0};	
	int len = 0;
	int ret = 0;
	pid_t pid;
	int wfd;
	
	len = sizeof(udpaddr);
//	printf("recvfrom beging...\n");
	ret = recvfrom(udpfd, name, sizeof(name), 0, (struct sockaddr*)&udpaddr, &len);
	if(ret == -1)
	{
		printf("recvfrom error\n");
		return;
	}
	//将数据写文件，传给新的窗口;
	forkbuf.tag = 'b';			//b 被动建立聊天
	memcpy(forkbuf.myname, myaccount, 10);
	memcpy(forkbuf.dfname, name, 10);
	memcpy(&forkbuf.dfaddr, &udpaddr, sizeof(struct sockaddr_in));
		
	wfd = open("chatdata", O_WRONLY | O_CREAT | O_TRUNC, 0640);
	ret = write(wfd, &forkbuf, sizeof(forkbuf));	
	if(ret == -1)
	{
		printf("0 write error\n");
		return;
	}
	
	
	close(wfd);
	
	pid = fork();
	if(pid == 0)
	{
		execl("/usr/bin/gnome-terminal", "gnome-terminal", "-e", "./chatwindow/chat", NULL);
	}
	if(pid == -1)
	{
		printf("udp read fork error\n");
		return;
	}
	return;
}








/*
*函数名：create_chat
*功能：建立udp聊天
*入参：udpfd
*返回值：无
*其他：打开新的终端执行聊天
*/
void create_chat(int udpfd)		//tcpfd 暂时
{
	struct friends *frp = frhead;
	struct forkdata forkbuf;
	char name[11] = {0};		//
	char buf[1024] = {0};
	int len = 0;
	pid_t pid;
	int wfd;
	int ret = 0;

	printf("请输入对方的帐号：");
	fgets(name, sizeof(name), stdin);
	name[strlen(name) - 1] = '\0';

	while(frp != NULL)
	{
		if(strcmp(name, frp->account) == 0)
		{
		//	printf("connecttion from host %s ,port %d,\n", inet_ntoa(frp->dfaddr.sin_addr), ntohs(frp->dfaddr.sin_port));
			
			//写文件传给新开程序
			forkbuf.tag = 'z';
			memcpy(forkbuf.myname, myaccount, 10);
			memcpy(forkbuf.dfname, frp->account, 10);
			memcpy(&forkbuf.dfaddr, &frp->dfaddr, sizeof(struct sockaddr_in));
			
			wfd = open("chatdata", O_WRONLY | O_CREAT | O_TRUNC, 0640);
			if(wfd == -1)
			{
				printf("jianli  open error\n");
				return;
			}
			
			ret = write(wfd, &forkbuf, sizeof(forkbuf));		// ‘z’表示主动建立聊天
			if(ret == -1)
			{
				printf("jian 0 write error\n");
				return;
			}
			close(wfd);
			
			//建立进程
			pid = fork();
			if(pid == 0)
			{
				execl("/usr/bin/gnome-terminal", "gnome-terminal", "-e", "./chatwindow/chat", NULL);
			}
			if(pid == -1)
			{
				printf("create fork error\n");
				return;
			}
			printf("窗口已开\n");
			return;
		}

		frp = frp->next;
	}
	if(frp == NULL)
	{
		printf("没有这个好友，或者对方不在线\n");
		return;
	}

}
