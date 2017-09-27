#include "treat.h"

struct online* olhead = NULL;		// online link 已经登录链表
struct online* linkhead = NULL;		// 已建立tcp链接 等待登录link




/*
*函数名：read_data
*功能：读取网口数据
*入参：fd 套接字
*返回值：0 断开连接
*其他：无
*/
int read_data(int fd)
{

	struct protocol datum = {0};
	struct online *olp = olhead;
	struct online *olt = NULL;
	int ret = 0;
	//
	printf("beging read...\n");
	ret = read(fd, &datum.head, 2);
	if(ret == -1)
	{
		printf("read error\n");
		return -1;
	}
	if(ret == 0)
	{
		printf("link close\n");
		//删除在线节点
		while(olp != NULL)
		{
			if(olp->accfd == fd)
			{
				printf("deldeldleldldld\n");///////
				if(olp == olhead)
				{
					olhead = olp->next;
				}
				else
				{
					olt->next = olp->next;
				}
				//
				printf("send_offline\n");
				//
				send_offline(olp);
				//
				printf("send_offline over\n");
				close(olp->accfd);
				free(olp);
				break;
			}
			olt = olp;
			olp = olp->next;
		}
		if(olp == NULL)
		{
			olp = linkhead;
			olt = NULL;
			while(olp != NULL)
			{
				if(olp->accfd == fd)
				{
					if(olp == linkhead)
					{
						linkhead = olp->next;
					}
					else
					{
						olt->next = olp->next;
					}
					close(olp->accfd);
					free(olp);
					break;
				}
				olt = olp;
				olp = olp->next;
			}
		}
		
		//打印在线链表
		print_online();
		return 0;
	}
	//
	printf("read len...\n");
	read(fd, &datum.len, 2);
	read(fd, datum.data, datum.len);
	
	
	switch(datum.head)
	{
		case LOGIN_DATA:
			//登陆处理函数
			printf("处理登陆\n");
			login_treat(fd, &datum);
			break;
		case REGISTER_DATA:
			//注册处理函数
			printf("处理注册\n");
			treat_register(fd, &datum);
			break;
		case PANT_DATA:
			//xingtiao
			printf("*pant*\n");
			treat_pant(fd);
			break;
		default:
			printf("error message\n");
	}

	return 1;
}



/*
*函数名：login_treat
*功能：处理登陆数据
*入参：
*返回值：
*其他：无
*/
void login_treat(int fd, struct protocol* datum)
{
	int file_fd = 0;
	int ret = 0;
	struct account accbuf = {0};
	struct protocol probuf = {0};

	probuf.head = LOGIN_DATA;
	probuf.len = 1;

	printf("登陆的name：%s\n", datum->data); ///////测试用
	file_fd = open("account", O_RDONLY);
	if(file_fd > -1)
	{
		while(1)
		{
			ret = read(file_fd, &accbuf, sizeof(accbuf));
			if(ret > 0)
			{
				if(strcmp(accbuf.username, (char*)datum->data) == 0)
				{
					if(strcmp(accbuf.password, datum->data + 10) == 0)
					{
						printf("登陆成功\n");
						probuf.data[0] = 1;
						write(fd, &probuf, 2 + 2 + 1);
						//添加在线信息
					//	add_online(fd, datum);
						add_online(fd, datum);
						print_online();	//打印

						close(file_fd);
						return;
					}
				//	break;
				}
				continue;
			}
			else if(ret == 0)
			{
				break;
			}
		}
	}
	probuf.data[0] = 0;
	write(fd, &probuf, 2 + 2 + 1);

	close(file_fd);
	return;
}




/*
*函数名：treat_register
*功能：处理注册信息
*入参：
*返回值：
*其他：
*/
void treat_register(int fd, struct protocol* datum)
{
	int file_fd = 0;
	int ret = 0;
	struct protocol probuf = {0};
	struct account accbuf = {0};
	char name[10] = {0};
	

	memcpy(name, datum->data, 10);
	printf("register name: %s\n", name);
	probuf.head = REGISTER_DATA;
	probuf.len = 1;

	file_fd = open("account", O_RDWR | O_APPEND | O_CREAT, 0640);
	if(file_fd == -1)
	{
		printf("open error in register function\n");
		return;
	}
	while(1)
	{
		printf("read file 33line \n");
		ret = read(file_fd, &accbuf, sizeof(accbuf));
		if(ret > 0)
		{
			if(strcmp(accbuf.username, name) == 0)
			{
				if(strcmp(accbuf.password, datum->data + 10) == 0)
				{
					printf("账户名已存在，注册失败\n");
					probuf.data[0] = 0;
					write(fd, &probuf, 2 + 2 + 1);
					close(file_fd);
					return;
				}

			}
			continue;
		}
		else if(ret == 0)
		{
			break;
		}
	}
	
	memcpy(&accbuf, datum->data, 20);
	
	ret = write(file_fd, &accbuf, sizeof(accbuf));
	if(ret == -1)
	{
		printf("write error\n");
	}
	else
	{
		probuf.data[0] = 1;
		write(fd, &probuf, 2 + 2 + 1);
	}
	close(file_fd);
	return;
}



/*
*函数名：treat_pant
*功能：处理接收到的心跳数据
*入参：
*返回值：
*其他：
*/
void treat_pant(int fd)
{
	struct online *olp = olhead;

	while(olp != NULL)
	{
		if(fd == olp->accfd)
		{
			olp->time = 0;
			return;
		}
		olp = olp->next;
	}
}





/*
*函数名：send_offline
*功能：发送下线通知
*入参：无
*返回值：无
*其他：无
*/
void send_offline(struct online* offline)
{
	struct online* olp = olhead;
	struct protocol probuf = {0};

	probuf.head = OFFLINE_DATA;
	probuf.len = 10;

	memcpy(probuf.data, offline->account, 10);
	while(olp != NULL)
	{	
		write(olp->accfd, &probuf, 2 + 2 + 10);
		olp = olp->next;
	}
}



/*
*函数名：add_online
*功能：添加上线节点 send offline to other account
*入参：无
*返回值：无
*其他：无
*/
void add_online(int fd, struct protocol* datum)
{
	struct online* linkp = linkhead;
	struct online* linkt = NULL;
	struct online* olp = olhead;

	struct protocol probuf = {0};


	probuf.head = ONLINE_DATA;
	probuf.len = 0;

	while(olp != NULL)
	{
		// 一次发所有的 在线信息	
		memcpy(probuf.data + probuf.len, olp->account, 10);
		memcpy(probuf.data + probuf.len + 10, &olp->accaddr, sizeof(struct sockaddr_in));
		probuf.len += sizeof(struct sockaddr_in) + 10;
		if(probuf.len > 1020 - sizeof(struct sockaddr_in))
		{
			write(fd, &probuf, probuf.len + 4);
			
		//	memset(&probuf, 0, sizeof(probuf));
		//	probuf.head = ONLINE_DATA;
			probuf.len = 0;
		}
		olp = olp->next;
	}
	write(fd, &probuf, probuf.len + 4);
	
	olp = olhead;
	while(linkp != NULL)
	{
		if(linkp->accfd == fd)
		{
			memcpy(linkp->account, datum->data, 10);
			//
			printf("login account:%s\n", linkp->account);
			//将新登陆用户信息发送给其它用户
			memcpy(probuf.data, linkp->account, 10);
			memcpy(probuf.data + 10, &linkp->accaddr, sizeof(struct sockaddr_in));
			probuf.len = sizeof(struct sockaddr_in) + 10;
			probuf.head = ONLINE_DATA;
			while(olp != NULL)
			{
				write(olp->accfd, &probuf, probuf.len + 4);
				olp = olp->next;
			}
			//发送结束

			if(linkp == linkhead)
			{
				linkhead = linkhead->next;
			}
			else
			{
				linkt->next = linkp->next;
			}

			linkp->next = olhead;
			olhead = linkp;
		}
		linkt = linkp;
		linkp = linkp->next;
	}
}



/*
*函数名：print_online
*功能：打印在线信息
*入参：无
*返回值：无
*其他：无
*/
void print_online()
{
	struct online* olp = olhead;
	
	printf("\n\n");
	if(olhead == NULL)
	{
		printf("没有在线用户\n");
		return;
	}
	
	while(olp != NULL)
	{
		printf("account:%s,  fd:%d\n", olp->account, olp->accfd);
		olp = olp->next;
	}
	printf("\n");
	return;
}

