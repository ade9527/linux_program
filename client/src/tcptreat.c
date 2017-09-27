#include "common.h"
#include "tcptreat.h"

static int login_tag = 0;
static int register_tag = 0;

#define GETCH getchar(); getchar

/*
*函数名：read_data
*功能：读取tcp数据
*入参：tcpfd
*返回值：int
*其他：
*/
int read_data(int tcpfd)
{
	struct protocol datum = {0};
	int ret = 0;
	
//	printf("..beging read...\n");
	ret = read(tcpfd, &datum.head, 2);
	if(ret == -1)
	{
		printf("read error\n");
		return 0;
	}
	if(ret == 0)
	{
		printf("server error,break link!\n");
		close(tcpfd);
		exit(1);
	}

	read(tcpfd, &datum.len, 2);
	
	printf("read data...\n");
	read(tcpfd, datum.data, datum.len);
	
	printf("read switch\n");
	switch(datum.head)
	{
		case LOGIN_DATA:
			//
			if(datum.data[0] == 1)
			{
	//			printf("login success\n");
				//登陆成功界面
				login_tag = 1;
				return 11;
			}
			else
			{
	//			printf("login fail\n");
				login_tag = -1;
				return 10;
			}
			break;
		case  REGISTER_DATA:
			//
			if(datum.data[0] == 1)
			{
	//			printf("register success\n");
				register_tag = 1;
				return 21;	
			}
			else
			{
	//			printf("register fail\n");
				register_tag = -1;
				return 20;
			}
			break;
		case ONLINE_DATA:
			//
			add_online(&datum);
			break;
		case OFFLINE_DATA:
			//
			del_online(&datum);
			break;

		default :
			printf("数据有误\n");
			return -1;
	}

}






/*
*函数名：add_online
*功能：增加在线链表
*入参：read读到的服务器发来的数据
*返回值：无
*其他：头查
*/
void add_online(struct protocol *datum)
{
	struct friends* fs = NULL;
	int a = 10 + sizeof(struct sockaddr_in);
	int i;
	
	for(i = 0; i < datum->len; i += a)
	{
		fs = (struct friends*)malloc(sizeof(struct friends));
		if(fs == NULL)
		{
			printf("mallco error\n");
			exit(2);
		}

		memcpy(fs->account, datum->data + i, 10);
		memcpy(&fs->dfaddr, datum->data + i + 10, sizeof(struct sockaddr_in));
		
		fs->next = frhead;
		frhead = fs;
		
	}


}


/*
*函数名：del_online
*功能：删除下线节点
*入参：read读到的服务器发来的数据
*返回值：无
*其他：无
*/
void del_online(struct protocol *datum)  
{
	struct friends* ft = frhead;
	struct friends* fp = NULL;
	char name[10] = {0};

	printf("del offline\n");
	memcpy(name, datum->data, 10);
	printf("name :%s\n\n", name);
	while(ft != NULL)
	{
		if(strcmp(name, ft->account) == 0)
		{
			if(ft == frhead)
			{
				frhead = ft->next;
			}
			else
			{
				fp->next = ft->next;
			}
			free(ft);
			break;
		}
		fp = ft;
		ft = ft->next;
	}
	
}








/*
*函数名：user_login
*功能：发送登陆信息 函数
*入参：tcpfd，udpfd
*返回值：int
*其他：等待 登陆成功与否 10s
*/
int user_login(int tcpfd, int udpfd)
{
	struct protocol datum = {0};
	char buf[11];
	int ret = 0;
	int flag = 0;
	int n = 0;
	pthread_t pantid;

	datum.head = LOGIN_DATA;
	datum.len = 20;

	login_tag = 0;

	while(1)
	{
		printf("请输入帐号:");
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf) - 1] = '\0';
		memcpy(datum.data, buf, 10);
		
		printf("请输入密码:");
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf) - 1] = '\0';
		memcpy(datum.data + 10, buf, 10);

//printf("login name:%s, password:%s,\n", datum.data, datum.data + 10);

		ret = write(tcpfd, &datum, 2 + 2 + 20);
		if(ret == -1)
		{
			printf("write error\n");
			exit(1);
		}

		printf("landing...\n");
		while(1)
		{
			if(login_tag == 1)
			{
				printf("登陆成功\n");
				login_tag = 0;
				memcpy(myaccount, datum.data, 10);
				//
				ret = pthread_create(&pantid, NULL, (void*)send_pant, (void*)tcpfd);
				if(ret != 0)
				{
					printf("create pant pthread error\n");
					return -1;
				}
				
				return 1;
			}
			if(login_tag == -1)
			{
				login_tag = 0;

				printf("登陆失败!账户或密码有误\n");
				printf("1-退出登陆\n");
				printf("其它键重新输入\n");
				flag = GETCH();

				if(flag == '1')
				{
					return -2;
				}
				else
				{
					break;
				}
			}
			if(login_tag == 0)
			{
				if(n > 10)
				{
					printf("登陆超时，请检查您的网络是否通畅\n");
					
					return -1;
				}
				n++;
				sleep(1);
			}
		}
	}
}



/*
*函数名：apply_register
*功能：发送注册信息函数
*入参：tcpfd
*返回值：int
*其他：等待 注册成功与否 10s
*/
int apply_register(int tcpfd)
{
	struct protocol datum = {0};
	char buf[11];
	int ret = 0;
	int flag = 0;
	int n = 0;

	datum.head = REGISTER_DATA;
	datum.len = 20;
	
	register_tag = 0;		//tag 清零
	
	while(1)
	{
		printf("请输入帐号:");
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf) - 1] = '\0';
		memcpy(datum.data, buf, 10);
		
		printf("请输入密码:");
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf) - 1] = '\0';
		memcpy(datum.data + 10, buf, 10);

		//测试
	//	printf("login name:%s, password:%s,\n", datum.data, datum.data + 10);
		
		ret = write(tcpfd, &datum, 2 + 2 + 20);
		if(ret == -1)
		{
			printf("write error\n");
			exit(1);
		}

		printf("landing...\n");
		while(1)
		{
			if(register_tag == 1)
			{
				printf("注册成功\n");
				register_tag = 0;
				//
				return 1;
			}
			if(register_tag == -1)
			{
				register_tag = 0;

				printf("注册失败!账户已存在\n");
				printf("1-退出注册\n");
				printf("其它键重新输入\n");
				flag = GETCH();
				if(flag == '1')
				{
					return 2;
				}
				else
				{
					break;
				}
			}
			if(register_tag == 0)
			{
				if(n > 10)
				{
					printf("请求超时，请检查您的网络是否通畅\n");
					
					return -1;
				}
				printf("n ++\n");
				n++;
				sleep(1);
			}
	//		printf("cuowe.............\n");
		}
	}
}





/*
*函数名：send_pant
*功能：发送心跳包每 5s
*入参：tcpfd
*返回值：无
*其他：无
*/
void send_pant(int tcpfd)
{
	struct protocol datum = {0};

	datum.head = PANT_DATA;
	datum.len = 0;

	while(1)
	{
		write(tcpfd, &datum, 2 + 2);
		sleep(5);
	}
}

