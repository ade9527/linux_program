#include "client.h"






/*
*函数名：main
*功能：入口函数
*入参：无
*返回值：int
*其它：安装信号
*/
int main()
{
	int ret = 0;
	
	signal(SIGCHLD, sig_routine);
	ret = client();
	if(ret == -1)
	{
		return -1;
	}
	return 0;
}



/*
*函数名：client
*功能：建立套接字，监听事件发生，
*入参：无
*返回值： -1 失败
*其它：建立线程现实菜单
*/
int client()
{
	int tcpfd = 0;
	int udpfd = 0;
	int ret = 0;
	int len = 0;
	int nfound = 0;
	int sfd[2] = {0};
	
	pthread_t menuid;
	pthread_t pantid;

	struct sockaddr_in seraddr = {0};
	
	fd_set rset, set; 
	int sock, maxfd;

	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons((uint16_t)SER_PORT);
	inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr.s_addr);
	
	printf("socket...\n");
	tcpfd = socket(AF_INET, SOCK_STREAM, 0);
	if(tcpfd == -1)
	{
		printf("socket error\n");
		return -1;
	}
	
	udpfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(udpfd == -1)
	{
		printf("socket udp error\n");
		close(tcpfd);
		return -1;
	}
	


	printf("connect...\n");
	ret = connect(tcpfd, (struct sockaddr*)&seraddr, sizeof(seraddr));
	if(ret == -1)
	{
		printf("connect error\n");
		close(tcpfd);
		close(udpfd);
		return -1;
	}
	
	bind_udp_port(tcpfd, udpfd);		//udpfd 绑定成 tcpfd 的port


	sfd[0] = tcpfd;
	sfd[1] = udpfd;
	ret = pthread_create(&menuid, NULL, (void*)display_menu, (void*)sfd);
	if(ret != 0)
	{
		printf("create menu pthread error\n");
		return -1;
	}

	FD_ZERO(&set);
	FD_SET(tcpfd, &set);
	FD_SET(udpfd, &set);
	maxfd = (tcpfd > udpfd ? tcpfd : udpfd);
	
	printf("tcpfd %d, udpfd %d, maxfd %d\n", tcpfd, udpfd, maxfd);
	
	//select 监听事件发生；
	while(1)
	{
		rset = set;
	//	printf("select...\n");
		nfound = select(maxfd + 1, &rset, NULL, NULL, NULL);
		if(nfound == -1)
		{
			if(errno == EINTR)
			{
				printf("interrupted system call\n");
				continue;
			}
			printf("select error\n");
			close(tcpfd);
			close(udpfd);
			exit(1);
		}

		if(FD_ISSET(tcpfd, &rset))
		{
			printf("sele read ..\n");
			ret = read_data(tcpfd);
		//接收数据处理函数
		}
		if(FD_ISSET(udpfd, &rset))
		{
			printf("udp message\n");
			udp_read(udpfd);	//读udp数据
		}

	}
	return 0;

}






/*
*函数名：sig_routine
*功能：接收子进程退出信号
*入参：
*返回值：无
*其它：防止僵尸进程
*/
static void sig_routine(int signo)
{
	if(signo == SIGCHLD)
	{
		printf("in sig routine\n");
		while(waitpid(-1, NULL, WNOHANG) > 0);
	}
	return;
}


