## 实验一、编译Linux内核
## 实验二、进程控制
    设计并实现Unix的“time”命令。“mytime”命令通过命令行参数接受要运行的程序，创建一个独立的进程来运行该程序，并记录程序运行的时间。
在Windows下实现:
	使用CreateProcess()来创建进程
	使用WaitForSingleObject()在“mytime”命令和新创建的进程之间同步
	调用GetSystemTime()来获取时间
在Linux下实现:
	使用fork()/vfork /exec()来创建进程运行程序
	使用wait()等待新创建的进程结束
	调用gettimeofday()来获取时间
	mytime的用法：
	$ mytime.exe program1
	要求输出程序program1运行的时间。Pragram1可以为自己写的程序，也可以是系统里的应用程序。
	$ mytime.exe program2 t
	t为时间参数，为program2的输入参数，控制program2的运行时间。最后输出program2的运行时间，输出结果应和t基本接近。
显示结果： * *小时* *分* *秒* *毫秒* *微秒

## 实验三、生产者消费者问题
•一个大小为3的缓冲区，初始为空
•2个生产者
	–随机等待一段时间，往缓冲区添加数据，
	–若缓冲区已满，等待消费者取走数据后再添加
	–重复6次
•3个消费者
	–随机等待一段时间，从缓冲区读取数据
	–若缓冲区为空，等待生产者添加数据后再读取
	–重复4次
说明：
	•显示每次添加和读取数据的时间及缓冲区里的数据
	•生产者和消费者用进程模拟 

## 实验四、内存监视
设计一个内存监视器，能实时地显示当前系统中内存的使用情况，包括系统地址空间的布局，物理内存的使用情况；能实时显示某个进程的虚拟地址空间布局和工作集信息等。
相关的系统调用：
	GetSystemInfo, VirtualQueryEx, GetPerformanceInfo, GlobalMemoryStatusEx …

## 实验五、复制文件
完成一个目录复制命令mycp，包括目录下的文件和子目录, 运行结果如下：
	beta@bugs.com [~/]# ls –l sem
	total 56
	drwxr-xr-x  3 beta beta 4096 Dec 19 02:53 ./
	drwxr-xr-x  8 beta beta 4096 Nov 27 08:49 ../
	-rw-r--r--  1 beta beta  128 Nov 27 09:31 Makefile
	-rwxr-xr-x  1 beta beta 5705 Nov 27 08:50 consumer*
	-rw-r--r--  1 beta beta  349 Nov 27 09:30 consumer.c
	drwxr-xr-x  2 beta beta 4096 Dec 19 02:53 subdir/
	beta@bugs.com [~/]# mycp sem target
	beta@bugs.com [~/]# ls –l target
	total 56
	drwxr-xr-x  3 beta beta 4096 Dec 19 02:53 ./
	drwxr-xr-x  8 beta beta 4096 Nov 27 08:49 ../
	-rw-r--r--  1 beta beta  128 Nov 27 09:31 Makefile
	-rwxr-xr-x  1 beta beta 5705 Nov 27 08:50 consumer*
	-rw-r--r--  1 beta beta  349 Nov 27 09:30 consumer.c
	drwxr-xr-x  2 beta beta 4096 Dec 19 02:53 subdir/

说明：
	Linux: creat，read，write等系统调用，要求支持软链接
	Windows: CreateFile(), ReadFile(), WriteFile(), CloseHandle()等函数
	特别注意复制后，不仅权限一致，而且时间属性也一致。
