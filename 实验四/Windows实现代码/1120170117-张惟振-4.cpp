#include <iostream>
#include <Windows.h>
#include <string>
#include <Shlwapi.h>
#include <Psapi.h>
#include <TlHelp32.h> //使用PROCESSENTRY32结构体
#include <iomanip>
#pragma comment(lib, "Shlwapi.lib")
using namespace std;
void ShowPerformanceInfo() //获取性能信息
{
	PERFORMANCE_INFORMATION pi;
	pi.cb = sizeof(pi);
	GetPerformanceInfo(&pi, sizeof(pi));
	cout << "性能信息：" << endl;
	cout << left << setw(30) << "系统当前提交的页面总数为：" << pi.CommitTotal << endl;
	cout << left << setw(30) << "系统当前可提交的最大页面数为：" << pi.CommitLimit << endl;
	cout << left << setw(30) << "系统历史提交页面峰值为：" << pi.CommitPeak << endl;
	cout << left << setw(30) << "按页分配的总物理内存为：" << pi.PhysicalTotal << endl;
	cout << left << setw(30) << "当前可用的物理内存为：" << pi.PhysicalAvailable << endl;
	cout << left << setw(30) << "系统缓存的容量为：" << pi.SystemCache << endl;
	cout << left << setw(30) << "内存总量为：" << pi.KernelTotal << endl;
	cout << left << setw(30) << "分页池的大小为：" << pi.KernelPaged << endl;
	cout << left << setw(30) << "非分页池的大小为：" << pi.KernelNonpaged << endl;
	cout << left << setw(30) << "页的大小为：" << pi.PageSize / 1024 << "KB" << endl;
	cout << left << setw(30) << "打开的句柄个数为：" << pi.HandleCount << endl;
	cout << left << setw(30) << "进程个数为：" << pi.ProcessCount << endl;
	cout << left << setw(30) << "线程个数为：" << pi.ThreadCount << endl;
	cout << endl
		<< endl;
	return;
}
void ShowMemoryInfo() //获取内存信息
{
	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof(ms);
	GlobalMemoryStatusEx(&ms);
	cout << "内存信息：" << endl;
	cout << left << setw(25) << "内存的使用率为：" << ms.dwMemoryLoad << "%" << endl;
	cout << left << setw(25) << "内存的总容量为：" << fixed << setprecision(2) << (float)ms.ullTotalPhys / 1024 / 1024 / 1024 << "GB" << endl;
	cout << left << setw(25) << "可用的内存为：" << fixed << setprecision(2) << (float)ms.ullAvailPhys / 1024 / 1024 / 1024 << "GB" << endl;
	cout << left << setw(25) << "页文件的总容量为：" << fixed << setprecision(2) << (float)ms.ullTotalPageFile / 1024 / 1024 / 1024 << "GB" << endl;
	cout << left << setw(25) << "可用的页文件为：" << fixed << setprecision(2) << (float)ms.ullAvailPageFile / 1024 / 1024 / 1024 << "GB" << endl;
	cout << left << setw(25) << "虚拟内存的总容量为：" << fixed << setprecision(2) << (float)ms.ullTotalVirtual / 1024 / 1024 / 1024 << "GB" << endl;
	cout << left << setw(25) << "可用的虚拟内存为：" << fixed << setprecision(2) << (float)ms.ullAvailVirtual / 1024 / 1024 / 1024 << "GB" << endl;
	cout << left << setw(25) << "可用的扩展虚拟内存为：" << fixed << setprecision(2) << (float)ms.ullAvailExtendedVirtual / 1024 << "KB" << endl;
	cout << endl
		<< endl;
	return;
}
void ShowSystemInfo() //获得系统信息
{
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);
	cout << "系统信息：" << endl;
	cout << left << setw(41) << "内存页的大小为：" << (int)si.dwPageSize / 1024 << "KB" << endl;
	cout << left << setw(41) << "每个进程可用地址空间的最小内存地址为：   0x" << setw(8) << si.lpMinimumApplicationAddress << endl;
	cout << left << setw(41) << "每个进程可用地址空间的最大内存地址为：   0x" << setw(8) << si.lpMaximumApplicationAddress << endl;
	cout << left << setw(41) << "系统配备的CPU的数量为：" << si.dwNumberOfProcessors << endl;
	cout << left << setw(41) << "能够保留地址空间区域的最小单位为：" << si.dwAllocationGranularity / 1024 << "KB" << endl;
	cout << endl
		<< endl;
	return;
}
void ShowProcessInfo() //获得进程信息
{
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	//获取当前运行进程的快照
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//获取第一个进程的句柄
	bool b = Process32First(hProcess, &pe);
	if (b == false)
		cout << "false" << endl;
	string pid = "PID";
	string pname = "进程名称";
	string psize = "进程工作集大小";
	cout << "进程信息：" << endl;
	printf("\n%5s%70s%25s\n\n", pid.c_str(), pname.c_str(), psize.c_str()); //进程ID，进程名称，进程工作集大小
	while (b)                                  //遍历所有进程
	{
		HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		PROCESS_MEMORY_COUNTERS pmc;
		ZeroMemory(&pmc, sizeof(pmc));
		if (GetProcessMemoryInfo(h, &pmc, sizeof(pmc)))
		{ //获取进程的使用情况
			printf("%5d%70ls%25.2fKB\n", pe.th32ProcessID, pe.szExeFile, ((float)pmc.WorkingSetSize) / 1024);
		}
		b = Process32Next(hProcess, &pe);
	}
	CloseHandle(hProcess);
	cout << endl
		<< endl;
	return;
}
//显示保护标记，该标记表示允许应用程序对内存进行访问的类型，参考课本292页
inline bool TestSet(DWORD dwTarget, DWORD dwMask)
{
	return ((dwTarget & dwMask) == dwMask);
}
#define SHOWMASK(dwTarget, type)        \
    if (TestSet(dwTarget, PAGE_##type)) \
    {                                   \
        cout << "," << #type;           \
    }
void ShowProtection(DWORD dwTarget)//显示当前块页面保护方式
{ //定义的页面保护方式
	SHOWMASK(dwTarget, READONLY);//只读
	SHOWMASK(dwTarget, GUARD);//保护
	SHOWMASK(dwTarget, NOCACHE);//无缓存
	SHOWMASK(dwTarget, READWRITE);//读写
	SHOWMASK(dwTarget, WRITECOPY);//写时复制
	SHOWMASK(dwTarget, EXECUTE);//执行
	SHOWMASK(dwTarget, EXECUTE_READ);//执行读
	SHOWMASK(dwTarget, EXECUTE_READWRITE);//执行读写
	SHOWMASK(dwTarget, EXECUTE_WRITECOPY);//执行写时复制
	SHOWMASK(dwTarget, NOACCESS);//未访问
}
void ShowVirtualMemoryInfoAndWorkingSetInfo() //获取某一进程的虚拟地址空间布局和工作集信息
{
	int pid;
	cout << "请输入进程PID：" << endl;
	cin >> pid;
	if (pid == -1)
	{
		cout << endl << endl;
		return;
	}
	HANDLE hProcess;
	while (1) //判断输入的PID是否正确
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		if (hProcess == NULL)
			cout << "PID错误，请重新输入：" << endl;
		else
			break;
		cin >> pid;
		if (pid == -1)
		{
			cout << endl << endl;
			return;
		}
	}
	cout << endl << "查看" << pid << "号进程的虚拟空间布局和工作集信息：" << endl << endl;
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);
	MEMORY_BASIC_INFORMATION mbi;  //进程虚拟内存空间的基本信息结构
	ZeroMemory(&mbi, sizeof(mbi)); //分配缓冲区，用于保存信息

	LPCVOID pBlock = (LPCVOID)si.lpMinimumApplicationAddress;
	for (; pBlock < si.lpMaximumApplicationAddress;) //遍历整个程序的地址空间
	{
		if (VirtualQueryEx(hProcess,                    //相关的进程
			pBlock,                      //开始位置
			&mbi,                        //缓冲区
			sizeof(mbi)) == sizeof(mbi)) //长度的确认
		{                                               //查询地址空间中内存地址的信息
			//计算块的结尾及其长度
			LPCVOID pEnd = (PBYTE)pBlock + mbi.RegionSize; //页基地址+虚存区大小
			TCHAR szSize[MAX_PATH];
			StrFormatByteSize(mbi.RegionSize, szSize, MAX_PATH); //格式化文件的大小
			//显示块地址和长度
			printf("块地址： %8X-%8X(%ls)     ", (DWORD)pBlock, (DWORD)pEnd, szSize);
			int l = lstrlen(szSize);
			int x = 10 - l;
			for (int i = 0; i < x; i++)//对齐格式
				printf(" ");
			//显示块的状态
			if (mbi.State == MEM_COMMIT)
				cout << "被提交";
			else if (mbi.State == MEM_FREE)
				cout << "空闲的";
			else if (mbi.State == MEM_RESERVE)
				cout << "被保留";
			//显示保护
			if (mbi.Protect == 0 && mbi.State != MEM_FREE)
				mbi.Protect = PAGE_READONLY;
			ShowProtection(mbi.Protect);
			//显示类型
			if (mbi.Type == MEM_IMAGE)
				cout << ",Image";
			else if (mbi.Type == MEM_PRIVATE)
				cout << ",Private";
			else if (mbi.Type == MEM_MAPPED)
				cout << ",Mapped";
			//检验可执行的映像
			TCHAR szFilename[MAX_PATH];            //提取用于当前可执行文件的文件名
			if (GetModuleFileName((HMODULE)pBlock, //实际虚拟内存的模块句柄
				szFilename,      //完全指定的文件名称
				MAX_PATH) > 0)   //实际使用的缓冲区长度
			{
				PathStripPath(szFilename); //除去路径
				cout << ",Module:" << szFilename;
			}
			cout << endl;
			pBlock = pEnd; //移动指针以获得下一个块
		}
	}
	PROCESS_MEMORY_COUNTERS p;
	ZeroMemory(&p, sizeof(p));
	GetProcessMemoryInfo(hProcess, &p, sizeof(p));
	cout << endl << "          工作集信息：" << (float)p.WorkingSetSize / 1024 << "KB" << endl;
	cout << endl << endl;
}
int main()
{
	while (1)
	{
		cout << "菜单" << endl
			<< "输入数字以使用对应的功能：" << endl
			<< "1：查看性能信息" << endl
			<< "2：查看内存状态" << endl
			<< "3：查看系统信息" << endl
			<< "4：查看进程信息" << endl
			<< "5：查看某一进程的虚拟地址空间布局和工作集信息" << endl
			<< "-1：返回" << endl
			<< endl;
		int in;
		cin >> in;
		if (in == 1)
			ShowPerformanceInfo();
		else if (in == 2)
			ShowMemoryInfo();
		else if (in == 3)
			ShowSystemInfo();
		else if (in == 4)
			ShowProcessInfo();
		else if (in == 5)
			ShowVirtualMemoryInfoAndWorkingSetInfo();
		else if (in == -1)
			return 0;
		else
			cout << "输入不合法，请重试！" << endl;
	}
	return 0;
}