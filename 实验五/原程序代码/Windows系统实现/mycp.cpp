#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>

void CopyFile(char *sourcefile, char *objectfile) //复制文件
{
	WIN32_FIND_DATA lpfindfiledata;
	HANDLE hfindfile = FindFirstFile(sourcefile, &lpfindfiledata);
	HANDLE hsource = CreateFile(sourcefile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//文件名，访问模式，共享模式，安全属性，如何创建，属性，句柄
	HANDLE hobject = CreateFile(objectfile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	LONG size = lpfindfiledata.nFileSizeLow - lpfindfiledata.nFileSizeHigh; //计算长度
	//复制文件
	int *buffer = new int[size];
	DWORD temp;
	ReadFile(hsource, buffer, size, &temp, NULL);
	WriteFile(hobject, buffer, size, &temp, NULL);
	//设置时间属性一致
	SetFileTime(hobject, &lpfindfiledata.ftCreationTime, &lpfindfiledata.ftLastAccessTime, &lpfindfiledata.ftLastWriteTime);
	CloseHandle(hfindfile); //关闭句柄
	CloseHandle(hsource);
	CloseHandle(hobject);
}

void CopyDirectory(char *sourcefile, char *objectfile) //复制目录
{
	WIN32_FIND_DATA lpfindfiledata;
	char source[1000], object[1000];
	strcpy(source, sourcefile);
	strcpy(object, objectfile);
	strcat(source, "\\*.*");
	strcat(object, "\\");
	HANDLE hfindfile = FindFirstFile(source, &lpfindfiledata);
	for (; FindNextFile(hfindfile, &lpfindfiledata) != 0;) //遍历所有文件
	{
		if (lpfindfiledata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{ //若为目录
			if (strcmp(lpfindfiledata.cFileName, ".") != 0 && strcmp(lpfindfiledata.cFileName, "..") != 0)
			{ //若既不是当前目录也不是父目录，则创建新目录
				memset(source, 0, sizeof(source));
				strcpy(source, sourcefile);
				strcat(source, "\\");
				strcat(source, lpfindfiledata.cFileName);
				strcat(object, lpfindfiledata.cFileName);
				CreateDirectory(object, NULL);
				CopyDirectory(source, object); //递归调用,进入子目录继续复制
				//设置时间属性一致
				HANDLE hsourcefile = CreateFile(source, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
				HANDLE hobjectfile = CreateFile(object, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
				FILETIME creationtime, accesstime, writetime;
				GetFileTime(hsourcefile, &creationtime, &accesstime, &writetime);
				SetFileTime(hobjectfile, &creationtime, &accesstime, &writetime);
				strcpy(object, objectfile);
				strcat(object, "\\");
			}
		}
		else //若为文件
		{
			memset(source, 0, sizeof(source));
			strcpy(source, sourcefile);
			strcat(source, "\\");
			strcat(source, lpfindfiledata.cFileName);
			strcat(object, lpfindfiledata.cFileName);
			CopyFile(source, object);
			strcpy(object, objectfile);
			strcat(object, "\\");
		}
	}
	CloseHandle(hfindfile);
}

int main(int argc, char *argv[])
{
	WIN32_FIND_DATA lpfindfiledata;
	if (FindFirstFile(argv[1], &lpfindfiledata) == INVALID_HANDLE_VALUE) //查找文件
	{
		printf("findfirstfile error!\n");
		exit(0);
	}
	if (FindFirstFile(argv[2], &lpfindfiledata) == INVALID_HANDLE_VALUE)
	{
		CreateDirectory(argv[2], NULL); //创建目录文件
		printf("createdirectory succeed!\n");
	}
	CopyDirectory(argv[1], argv[2]);
	//设置目录时间属性一致
	HANDLE hsourcefile = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	HANDLE hobjectfile = CreateFile(argv[2], GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	FILETIME creationtime, accesstime, writetime;
	GetFileTime(hsourcefile, &creationtime, &accesstime, &writetime);
	SetFileTime(hobjectfile, &creationtime, &accesstime, &writetime);
	printf("copy completed!\n");
	return 0;
}
