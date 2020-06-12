#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <utime.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>

void CopySoftLink(char *sourcefile, char *objectfile) //复制软链接
{
    char buffer[2000];
    char oldpath[1000];
    getcwd(oldpath, sizeof(oldpath));
    strcat(oldpath, "/");
    memset(buffer, 0, sizeof(buffer));
    readlink(sourcefile, buffer, 2000);               //找到软链接的目标
    symlink(strcat(oldpath, sourcefile), objectfile); //创建新的软链接
    struct stat statbuf;
    lstat(sourcefile, &statbuf);
    //设置时间属性一致
    struct timeval timebuf[2]; //utimes需要使用timeval数组修改时间
    timebuf[0].tv_sec = statbuf.st_atime;
    timebuf[0].tv_usec = 0;
    timebuf[1].tv_sec = statbuf.st_mtime;
    timebuf[1].tv_usec = 0;
    lutimes(objectfile, timebuf);
}

void CopyFile(char *sourcefile, char *objectfile) //复制文件
{
    int source = open(sourcefile, 0); //打开文件
    struct stat statbuf;
    stat(sourcefile, &statbuf);
    int object = creat(objectfile, statbuf.st_mode); //创建一个文件，若该文件已存在，则会将其长度截为零
    //复制文件
    char buffer[2000];
    int temp;
    for (; (temp = read(source, buffer, 2000)) > 0;)
    {
        if (write(object, buffer, temp) != temp) //判断是否写入完全
        {
            printf("write error!\n");
            exit(0);
        }
    }
    //设置时间属性一致
    struct utimbuf timebuf;
    timebuf.actime = statbuf.st_atime;
    timebuf.modtime = statbuf.st_mtime;
    utime(objectfile, &timebuf);
    close(source); //关闭文件
    close(object);
}
void CopyDirectory(char *sourcefile, char *objectfile) //复制目录
{
    DIR *dir;
    struct stat statbuf;
    char source[1000], object[1000];
    strcpy(object, objectfile);
    strcat(object, "/");
    if ((dir = opendir(sourcefile)) == NULL)
    {
        printf("opendir errorII!\n");
        exit(0);
    }
    struct dirent *entry;                   //存储目录文件信息
    for (; (entry = readdir(dir)) != NULL;) //遍历所有文件
    {
        if (entry->d_type == 4) //若为目录
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            { //若既不是当前目录也不是父目录,则创建新目录
                memset(source, 0, sizeof(source));
                strcpy(source, sourcefile);
                strcat(source, "/");
                strcat(source, entry->d_name);
                strcat(object, entry->d_name);
                stat(source, &statbuf);
                mkdir(object, statbuf.st_mode);
                CopyDirectory(source, object); //递归调用，进入子目录继续复制
                //先复制，再设置时间
                //设置时间属性一致
                struct utimbuf timebuf;
                timebuf.actime = statbuf.st_atime;
                timebuf.modtime = statbuf.st_mtime;
                utime(object, &timebuf);
                strcpy(object, objectfile);
                strcat(object, "/");
            }
        }
        else if (entry->d_type == 10) //若为软链接
        {
            memset(source, 0, sizeof(source));
            strcpy(source, sourcefile);
            strcat(source, "/");
            strcat(source, entry->d_name);
            strcat(object, entry->d_name);
            CopySoftLink(source, object);
            strcpy(object, objectfile);
            strcat(object, "/");
        }
        else //若为文件
        {
            memset(source, 0, sizeof(source));
            strcpy(source, sourcefile);
            strcat(source, "/");
            strcat(source, entry->d_name);
            strcat(object, entry->d_name);
            CopyFile(source, object);
            strcpy(object, objectfile);
            strcat(object, "/");
        }
    }
    closedir(dir);
}
int main(int argc, char *argv[])
{
    DIR *dir;
    if (argc != 3)
    {
        printf("input error!\n");
        exit(0);
    }
    if ((dir = opendir(argv[1])) == NULL) //查找目录
    {
        printf("opendir error!\n");
        exit(0);
    }
    struct stat statbuf;                  //存储文件属性
    if ((dir = opendir(argv[2])) == NULL) //创建目录
    {

        stat(argv[1], &statbuf);
        mkdir(argv[2], statbuf.st_mode);
        printf("mkdir succeed!\n");
    }
    closedir(dir);
    CopyDirectory(argv[1], argv[2]);
    //修改时间属性
    struct utimbuf timebuf;
    timebuf.actime = statbuf.st_atime;
    timebuf.modtime = statbuf.st_mtime;
    utime(argv[2], &timebuf);
    printf("copy completed!\n");
    return 0;
}
