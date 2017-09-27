#include "server.h"

#define SER_PORT 8080

int main()
{
	int listenfd = 0;
	int connfd = 0;
	int sockfd = 0;
	int ret = 0;
	int i = 0;
	int len = 0;
	pthread_t pant_id;
 
	struct sockaddr_in seraddr = {0};
	struct sockaddr_in cliaddr = {0};
	
	int epoll_instance = 0;
	int nfound = 0;
	struct epoll_event ev, events[20];

	struct online *linkp = NULL;

	//储存自己的地址信息
	memset(&seraddr, 0, sizeof(seraddr));
	seraddr.sin_family  = AF_INET;
	seraddr.sin_port = htons((uint16_t)SER_PORT);
	seraddr.sin_addr.s_addr = INADDR_ANY;

	epoll_instance = epoll_create(255);

	printf("socket...\n");
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd == -1)
	{
		printf("listenfd error\n");
		return -1;
	}

	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epoll_instance, EPOLL_CTL_ADD, listenfd, &ev);

	printf("bind...\n");
	ret = bind(listenfd, (struct sockaddr*)&seraddr, sizeof(seraddr));
	if(ret == -1)
	{
		printf("bind error\n");

		close(listenfd);
		return -1;
	}

	printf("listen...\n");
	ret = listen(listenfd, 5);
	if(ret == -1)
	{
		printf("listen error\n");

		close(listenfd);
		return -1;
	}

	//新建线程
	printf("create pthread...\n");
	ret = pthread_create(&pant_id, NULL, (void*)test_pant, NULL);
	if(ret != 0)
	{
		printf("create pthread error\n");
		return -1;
	}



	printf("while...\n");
	while(1)
	{
		printf("epoll_wait...\n");
		nfound = epoll_wait(epoll_instance, events, 20, -1);
		if(nfound == 0)
		{
			printf(".");
			fflush(stdout);
			continue;
		}
		for(i = 0; i < nfound; i++)
		{
			if(events[i].data.fd == listenfd)		// 连接 请求
			{
				len = sizeof(cliaddr);
				connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);
				printf("connection from host %s ,port %d,connfd %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), connfd);
				
				ev.data.fd = connfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epoll_instance, EPOLL_CTL_ADD, connfd, &ev);
				//
				ret = write(connfd, &cliaddr, sizeof(cliaddr));
				if(ret == -1)
				{
					printf("send addr write error\n");
					return -1;
				}
				
				//建立连接链表
				linkp = (struct online*)malloc(sizeof(struct online));
				if(linkp == NULL)
				{
					printf("malloc error\n");
					return -1;
				}

				linkp->accfd = connfd;
				linkp->accaddr = cliaddr;
				linkp->time = 0;

				linkp->next = linkhead;
				linkhead = linkp;
				//

			}
			else if(events[i].events & EPOLLIN)	//读事件
			{
				printf("...read shijian..\n");
				//处理函数
				sockfd = events[i].data.fd;
				//数据处理函数 ret = read;
				ret = read_data(sockfd);
				if(ret == 0)
				{
					ev.data.fd = sockfd;
					ev.events = EPOLLIN | EPOLLET;
					epoll_ctl(epoll_instance, EPOLL_CTL_DEL, sockfd, &ev);
					//删除在线节点
					//发送下线信息
					
					close(sockfd);
					printf("del client\n");
				}
			}
		}
	}


	return 0;
}



/*
*函数名：test_pant
*功能：检测心跳函数
*入参：无
*返回值：无
*其他：无
*/
void test_pant()
{
	struct online *olp = olhead;
	struct online *olt = NULL;

	while(1)
	{
		olp = olhead;
		olt = NULL;
		while(olp != NULL)
		{
			if(olp->time > TIME_PANT)
			{
				
				//删除节点	
				printf("删除没有心跳的节点:%d\n", olp->accfd);
				if(olp == olhead)
				{
					olhead = olp->next;
				//发送下线信息
					send_offline(olp);

					close(olp->accfd);
					free(olp);
					olp = olhead;
				}
				else
				{
					olt->next = olp->next;
				//发送下线信息
					send_offline(olp);

					close(olp->accfd);
					free(olp);
					olp = olt->next;
				}
				print_online();	//打印在线

			}
			else
			{
				olp->time++;
				olt = olp;
				olp = olp->next;
			}
		}
		sleep(5);
	}
}
