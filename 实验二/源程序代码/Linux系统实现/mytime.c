#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/time.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	struct timeval start,end;
	pid=fork();
	if(pid<0)
	{
		printf("fork() error\n");
		exit(-1);
	}
	gettimeofday(&start,NULL);
	if(pid==0)
	{
		//在子进程中运行另一个程序
		if(execvp(argv[1],argv)<0)
		{
			printf("execvp() error\n");
			exit(-1);
		}
	}
	else
	{
		wait(0);
		gettimeofday(&end,NULL);
		//计算相差的总秒数和微秒数
		int seconds=end.tv_sec-start.tv_sec;	
		int useconds=end.tv_usec-start.tv_usec;	
		if(useconds<0)
		{
			useconds+=1000000;
			seconds-=1;
		}
		//计算时分秒毫秒微秒
		int h=seconds/3600;
		int min=(seconds-h*3600)/60;
		int s=seconds-h*3600-min*60;
		int ms=useconds/1000;
		int us=useconds-ms*1000;
		printf("运行时间：%d小时%d分钟%d秒%d毫秒%d微秒\n",h,min,s,ms,us);
	}
	return 0;
}
