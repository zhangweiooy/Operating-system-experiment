#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h>

struct buffer//定义缓冲区 
{
	int s[3];
	int head;
	int tail;
	int is_empty;
};

struct sharedmemory//定义共享内存 
{
	struct buffer data;
	HANDLE full;
	HANDLE empty;
	HANDLE mutex; 
};
//显示缓冲区数据
void showdata(struct sharedmemory* a)
{
	printf("当前缓冲区数据：");
	for(int i=a->data.head; ; )
	{
		printf("%d ",a->data.s[i]);
		i++;
		i%=3;
		if(i==a->data.tail)
		{
			printf("\n\n");
			return ;
		}
	}
}
//创建克隆进程 
PROCESS_INFORMATION StartClone(int id)
{
	TCHAR szFilename[MAX_PATH];
	TCHAR szCmdLine[MAX_PATH];
	PROCESS_INFORMATION pi;
	GetModuleFileName(NULL,szFilename,MAX_PATH);
	sprintf(szCmdLine,"\"%s\" %d",szFilename,id);
	STARTUPINFO si;   
	ZeroMemory(reinterpret_cast<void*>(&si),sizeof(si));
	si.cb=sizeof(si);
	//创建子进程
	BOOL bCreatOK=CreateProcess(szFilename,szCmdLine,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
	return pi; 
	
}
HANDLE MakeSharedFile()	//创建共享内存
{
	//创建一个临时文件映射对象
	HANDLE hMapping=CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,PAGE_READWRITE,0,sizeof(struct sharedmemory),"BUFFER");
	if(hMapping==NULL)
	{
		printf("CreateFileMapping error!\n");
		exit(0);
	}
	//在文件映射上创建视图，返回起始虚地址
	LPVOID pData=MapViewOfFile(hMapping,FILE_MAP_ALL_ACCESS,0,0,0);
	if(pData==NULL)
	{
		printf("MapViewOfFile error!\n");
		exit(0);
	}
	if(pData!=NULL)
		ZeroMemory(pData, sizeof(struct sharedmemory));
	UnmapViewOfFile(pData);  
	return(hMapping);
}
//获取一个n位的随机数
int getrandomnumber(int n)
{
	
	int temp=1;
	while(n--)
		temp*=10;
	int rn=rand();
	return rn%temp;
}
int main(int argc,char* argv[])
{
	int subid=0;
	HANDLE hMapping;
	if(argc>1)
	{
		//如果有参数就作为子进程id
		sscanf(argv[1],"%d",&subid); 
	 } 
	if(subid==0)//主进程 
	{
		hMapping=MakeSharedFile();
		//打开文件映射 
		HANDLE hFileMapping=OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,"BUFFER");
    	if(hFileMapping==NULL) 
		{
        	printf("OpenFileMapping error!\n");
        	exit(0);
    	}
    	LPVOID pFile=MapViewOfFile(hFileMapping,FILE_MAP_ALL_ACCESS,0,0,0);
    	if(pFile==NULL) 
		{
        	printf("MapViewOfFile error!\n");
        	exit(0);
    	}
		struct sharedmemory *addr=(struct sharedmemory*)(pFile);
    	addr->data.head=0;
    	addr->data.tail=0;
    	addr->data.is_empty=1;
    	addr->empty=CreateSemaphore(NULL,3,3,"EMPTY");
    	addr->full=CreateSemaphore(NULL,0,3,"FULL");
    	addr->mutex=CreateSemaphore(NULL,1,1,"MUTEX");
    	UnmapViewOfFile(pFile);
    	pFile=NULL;
    	CloseHandle(hFileMapping);
    	//创建子进程 
    	PROCESS_INFORMATION sub[5];
    	for(int i=0;i<5;i++)
			sub[i]=StartClone(++subid);
		//等待子进程结束
		for(int i=0;i<5;i++)
			WaitForSingleObject(sub[i].hProcess,INFINITE);
		//关闭子进程句柄
		for(int i=0;i<5;i++)
			CloseHandle(sub[i].hProcess);
	}
	else if(subid==1||subid==2)//生产者
	{
		HANDLE hMap=OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,"BUFFER"); 
		if(hMap==NULL) 
		{
        	printf("OpenFileMapping error!\n");
        	exit(0);
    	}
    	LPVOID pFile=MapViewOfFile(hMap,FILE_MAP_ALL_ACCESS,0,0,0);
    	if(pFile==NULL) 
		{
        	printf("MapViewOfFile error!\n");
        	exit(0);
    	}
		struct sharedmemory *addr=(struct sharedmemory*)(pFile);
    	addr->full=OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,"FULL");
    	addr->empty=OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,"EMPTY");
    	addr->mutex=OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,"MUTEX");	
    	for(int i=0;i<6;i++)
    	{
    		Sleep(getrandomnumber(3));
    		WaitForSingleObject(addr->empty,INFINITE);//P(empty)
    		WaitForSingleObject(addr->mutex,INFINITE);//P(mutex)
    		//向缓冲区添加数据
    		int num=getrandomnumber(3);
			addr->data.s[addr->data.tail]=num;
			addr->data.tail=(addr->data.tail+1)%3;
			addr->data.is_empty=0;
			SYSTEMTIME time;
			GetLocalTime(&time);
			printf("时间：%04d年%02d月%02d日%02d时%02d分%02d秒\n",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);
			printf("%d号生产者将%d放入缓冲区中\n",subid-1,num);
			showdata(addr);
			ReleaseSemaphore(addr->mutex,1,NULL);//V(mutex)
			ReleaseSemaphore(addr->full,1,NULL);//V(full)
		}
		UnmapViewOfFile(pFile);
		pFile=NULL;
		CloseHandle(hMap);
	}
	else if(subid==3||subid==4||subid==5)//消费者
	{
		HANDLE hMap=OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,"BUFFER"); 
		if(hMap==NULL) 
		{
        	printf("OpenFileMapping error!\n");
        	exit(0);
    	}
    	LPVOID pFile=MapViewOfFile(hMap,FILE_MAP_ALL_ACCESS,0,0,0);
    	if(pFile==NULL) 
		{
        	printf("MapViewOfFile error!\n");
        	exit(0);
    	}
		struct sharedmemory *addr=(struct sharedmemory*)(pFile);
    	addr->full=OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,"FULL");
    	addr->empty=OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,"EMPTY");
    	addr->mutex=OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,"MUTEX");	
    	for(int i=0;i<4;i++)
    	{
    		Sleep(getrandomnumber(3));
    		WaitForSingleObject(addr->full,INFINITE);//P(full)
    		WaitForSingleObject(addr->mutex,INFINITE);//P(mutex)
    
    		int num=addr->data.s[addr->data.head];
			addr->data.head=(addr->data.head+1)%3;
			if(addr->data.head==addr->data.tail)
				addr->data.is_empty=1;
			else
				addr->data.is_empty=0;
			SYSTEMTIME time;
			GetLocalTime(&time);
			printf("时间：%04d年%02d月%02d日%02d时%02d分%02d秒\n",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);
			printf("%d号消费者将%d从缓冲区取出\n",subid-3,num);
			if(addr->data.is_empty==0)
				showdata(addr);
			else
				printf("当前缓冲区为空\n\n");
			ReleaseSemaphore(addr->mutex,1,NULL);//V(mutex)
			ReleaseSemaphore(addr->empty,1,NULL);//V(empty)
		}
		UnmapViewOfFile(pFile);
		pFile=NULL;
		CloseHandle(hMap);
	}
	CloseHandle(hMapping);
	hMapping=INVALID_HANDLE_VALUE;
//	printf("end\n");
	return 0;
}
