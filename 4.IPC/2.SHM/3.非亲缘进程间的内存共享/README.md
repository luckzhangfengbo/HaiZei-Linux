## Linux下利用共享空间来实现两个没有亲缘关系的进程间通信
> 功能需求：
> 1.打开一个Linux命令窗口作为写入端->客户端（client）
> 2.打开另一个Linux命令窗口作为读取端->服务器（server）
> 3.在客户端（client）写入数据后服务器（server）可以读取到客户端信息并返回信息内容，如果客户端没有写入消息则服务器进入等待状态
> 4.用户输入quit进程结束

> 思路：
> 1.首先我们应该在客户端（client）和服务器（server）中创建一个可以共同使用的共享空间：
> 共享空间属于IPC对象中的一种，我们要实现两个没有亲缘关系的进程通信那就要求客户端和服务器传递的信息之间达成某种协议，这种协议可以使服务器收到客户端的写入内容，所以我们可以在共享空间创建时使用相同的key，这样客户端和服务器就可以访问同一个共享空间；

2.实现两个进程之间的同步：

> 共享空间里数据被读取时如果为空就会直接结束程序，所以为了使数据传输实现同步，就需要引入信号来实现；
> （1）首先使用信号让两个进程互相获取到进程号（pid），这就需要在创建共享空间地址的时候使里面的数据包括进程的进程号（pid）和发送的数据区；
> （2）然后当共享空间中没有数据时，服务器就等待客户端输入，当客户端输入消息后发送信号结束服务器的等待让服务器读取数据，服务器在读取数据时客户端进行等待，服务器读取完成后发送信号给客户端，此时客户端进行输入，服务器等待…然后依次循环进行从而实现进程之间的同步；

3.实现客户端（client）的输入功能和服务器（server）的接收读取功能：

> 定义一个全局的数据缓存区buf, 在客户端程序中先将数据写入到buf缓存区中, 再从缓存区传进共享空间，每次输入结束要将缓存区内容清空，方便下次输入；然后从服务器程序中去读取共享空间的内容；

```c
1.创建共享空间：
      它的组成信息有:
    	  提取码->key,  标识id号->shmid,  所有者->owner,  权限->perms,   大小->bytes,  共享进程数量->nattch;
     
      创建步骤所利用函数:
      	 1. 创建一个提取码ftok()
      	 2. 创建一个共享空间shmget()
      	 3. 设置共享空间映射内存shmat()
      	 4. 撤销共享内存映射shmdt()
      	 5. 删除一个共享内存空间shmctl()
      
      共享空间的地址类型：
      	typedef struct shm{
      		pid_t pid;
      		char buf[MAX];
      	}SHM;

    代码实现:
	```
	#include <sys/types.h>
	#include <sys/ipc.h>
	#include <sys/shm.h>
	
	#define SIZE 1024			//定义一个宏来表示共享空间的大小
	#define MAX 64				//定义一个宏来表示缓存区的大小
	
	typedef struct shm{			//定义共享空间里面需要包含的数据类型
       	    pid_t pid;
       	    char buf[MAX];
       	}SHM;

	int main()
	{
	    key_t  key;			//定义自己的共享空间的提取码
	    int shmid;				//定义一个共享空间的ID
	    SHM *shmaddr = NULL;		//定义共享空间的首地址

	    //创建一个提取码
	    if((key = ftok('.', 'z')) < 0)
	    {
	    	perror("ftok");			//创建失败就返回错误信息
	    	return  -1;
	    } 
	    
	    //创建一个共享空间
	    if((shmid = shmget(key, SIZE, IPC_CREAT  | 0666)) == -1)
	    {
	    	perror("shmget");
	    	return -1;
	    }

	    //设置共享空间的映射
	    if((shmaddr = shmat(shmid, NULL, 0)) == (SHM *)-1)		//-1的类型要和shmaddr类型一致需要进行类型强转
	    {
	    	perror("shmat");
	    	return -1;
	    }
	     
	    //删除共享空间
	    shmdt(shmaddr);
	    shmctl(shmid, IPC_RMID, NULL);
	```
	
2.实现两个进程之间同步：
	使用信号signal()函数来创建两个用户信号（SIGUSR）来向两个进程发送信号；
	用到的函数有：
		1.信号捕捉函数signal()
		2.等待信号函数pause()
		3.信号发送函数kill()
	
	信号捕捉函数用来注册用户信号，等待信号函数用来设定进程执行顺序，信号发送函数用来将信号发送给指定进程进行下一步操作；
这样使用三个信号函数就使两个进程之间产生同步通信；

    代码实现
	```
	client.c
		pid_t pid_server;
		
		//注册客户端信号
		signal(SIGUSR1, myfun);
		pid_server = shmaddr->pid;    		//获取服务器的PID				
		shmaddr->pid = getpid();		//把客户端的PID传到共享空间让服务器获取
		kill(pid_server, SIGUSR2);		//给服务器发信号

	server.c
		pid_t pid_client;
		
		//注册服务器信号
		signal(SIGUSR2, myfun);
		shmaddr->pid = getpid();		//把服务器的PID传到共享空间让客户端获取
		pause();				//等待客户端发送信号
		pid_client = shmaddr->pid;		//获取客户端的PID
	```
	
3.实现客户端（client）的输入功能和服务器（server）的接收读取功能：
     客户端：
	（1）使用fgets函数从标准输入获取到客户端输入的字符信息存入共享空间缓存区；
	（2）使用kill()函数给服务器发信号让它读取内容；
	（3）使用pause()函数等待服务器读取完毕的信号；
	（4）使用strncmp()函数来判断用户输入的退出信息结束来通信；
	（5）使用memset()函数来清空共享空间缓存区内容；
    服务器：
    	（1）使用pause()函数等待客户端输入消息完成的信号；
    	（2）使用strncmp()函数来判断用户输入的退出信息结束来通信；
    	（3）使用printf直接读取共享空间缓存区的内容；	
         (4)使用kill()函数给客户端发送信号表示读取完成；
         
    代码实现
	```
	client.c
		while(1)
		{
			printf("input>");
			fflush(stdout);
			fgets(shmaddr->buf, MAX, stdin);
			kill(pid, SIGUSR2);
			
			if(strncmp(shmaddr->buf, "quit", 4) == 0)
			      break;
			pause();
			memset(shmaddr->buf, 0, MAX);
		}

	server.c
		while(1)
		{
			pause();
			if(strncmp(shmaddr->buf, "quit", 4) == 0)
			      break;
			printf("read:%s", shmaddr->buf);
			kill(pid, SIGUSR1);
		}

	```
```



```c
    客户端：
    client.c
    	#include <stdio.h>
    	#include <stdlib.h>
    	#include <string.h>
    	#include <sys/types.h>
    	#include <sys/ipc.h>
    	#include <sys/shm.h>
    	#include <unistd.h>
    	#include <signal.h>

	#define SIZE 1024
	#dedine MAX 64

	typedef struct shm{
	      pid_t pid;
	      char buf[MAX];
	}SHM;

	void myfun(int sig)
	{
	      return ;
	}

	int main()
	{
	      key_t key;
	      SHM *shmaddr = NULL;
	      int shmid;
	      pid_t pid_server;
	      
	      //创建一个提取码
	      if((key = ftok(".", 'z')) == -1)
	      {
	      	    perror("ftok");
	      	    return -1;
	      }
	      
	      //创建共享空间
	      if((shmid = shmget(key, SIZE, IPC_CREAT | 0666)) == -1)
	      {
	      	    perror("shmget");
	      	    return -1;
	      }
	      
	      //设置共享空间地址
	      if((shmaddr = shmat(shmid, NULL, 0)) == (SHM *)-1)
	      {
	      	    perror("shmat");
	      	    return -1;
	      }
	      
	      //注册信号
	      signal(SIGUSR1, myfun);
	      pid_server = shmaddr->pid;
	      shmaddr->pid = getpid();
	      kill(pid_server, SIGUSR2);
	      
	      //写入发送程序
	      while(1)
	      {
	      	    printf("input>");
	      	    fflush(stdout);
	      	    fgets(shmaddr->buf, MAX, stdin);
	      	    kill(pid_server, SIGUSR2);
	      	    if(strncmp(shmaddr->buf, "quit", 4) == 0)
	      	    	    break;
	      	    pause();
	      	    memset(shmaddr->buf, 0, MAX);
	      }
	      
	      //删除共享空间
	      shmdt(shmaddr)；
	      shmctl(shmid, IPC_RMID, NULL);
	      return 0;
	}

服务器：
server.c
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <sys/types.h>
	#include <sys/ipc.h>
	#include <sys/shm.h>
	#include <unistd.h>
	#include <signal.h>
	
	#define SIZE 1024
	#define MAX 64
	
	typedef struct shm{
	      pid_t pid;
	      char buf[MAX];
	}SHM;
	
	void myfun(int sig)
	{
	      return ;
	}
	
	int main()
	{
	      key_t key;
	      SHM *shmaddr = NULL;
	      int shmid;
	      pid_t pid_client;
	      
	      //创建一个提取码
	      if((key = ftok(".", 'z')) == -1)
	      {
	      	    perror("ftok");
	      	    return -1;
	      }
	      
	      //创建一个共享空间
	      if((shmid = shmget(key, SIZE, IPC_CREAT | 0666) == -1)
	      {
	      	    perror("shmget");
	      	    return -1;
	      }
	      
	      //设置共享空间地址
	      if((shmaddr = shmat(shmid, NULL, 0) == (SHM *)-1)
	      {
	      	    perror("shmat");
	      	    return -1;
	      }
	      
	      //注册信号
	      signal(SIGUSR2, myfun);
	      shmaddr->pid = getpid();
	      pause();
	      pid_client = shmaddr->pid;
	      
	      //读取程序
	      while(1)
	      {
	      	    pause();
	      	    if(strncmp(shmaddr->buf, "quit", 4) == 0)
	      	    	  break;
	      	    printf("read:%s\n", shmaddr->buf);
	      	    kill(pid_client, SIGUSR1);
	      }
	      
	      //删除共享空间
	      shmdt(shmaddr);
	      shmctl(shmid, IPC_RMID, NULL);
	      return 0;
	}
```

