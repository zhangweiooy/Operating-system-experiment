#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <utime.h>

#define BUFF_SIZE 4096

void copyLink(char *source, char *destination)
{
    char oldpath[BUFF_SIZE];
    int rest = readlink(source, oldpath, BUFF_SIZE);
    int sy = symlink(oldpath, destination);
    struct stat statbuff;
    lstat(source, &statbuff);
    struct timeval times[2];
    times[0].tv_sec = statbuff.st_atime;
    times[0].tv_usec = 0;
    times[1].tv_sec = statbuff.st_mtime;
    times[1].tv_usec = 0;
    lutimes(destination, times);
}

void copyFile(char *source, char *destination)
{
    int sourcefile = open(source, O_RDONLY);
    if (sourcefile == -1)
    {
        printf("Can not open file %s\n", source);
        exit(0);
    }
    struct stat statbuff;
    lstat(source, &statbuff);
    int destinationfile = creat(destination, statbuff.st_mode);
    if (destinationfile == -1)
    {
        printf("Can not create file %s\n", destination);
        exit(0);
    }
    int word;
    char buff[BUFF_SIZE];
    while ((word = read(sourcefile, buff, BUFF_SIZE)) > 0)
    {
        if (write(destinationfile, buff, word) != word)
        {
            printf("Write error");
            exit(0);
        }
    }
    struct utimbuf timebuff;
    timebuff.actime = statbuff.st_atime;
    timebuff.modtime = statbuff.st_mtime;
    utime(destination, &timebuff);
    close(sourcefile);
    close(destinationfile);
}

void copyDir(const char *source, const char *destination)
{
    DIR *dir = opendir(source);
    struct dirent *entry;
    char tempsource[2048], tempdestination[2048];
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        if (entry->d_type == 4) //d_type为4表示是一个目录
        {
            strcpy(tempsource, source);
            strcpy(tempdestination, destination);
            strcat(tempsource, "/");
            strcat(tempdestination, "/");
            strcat(tempsource, entry->d_name);
            strcat(tempdestination, entry->d_name);
            //将源路径和目标路径都复制到temp中
            struct stat statbuff;
            lstat(tempsource, &statbuff);
            mkdir(tempdestination, statbuff.st_mode);
            struct utimbuf timebuff;
            timebuff.actime = statbuff.st_atime;
            timebuff.modtime = statbuff.st_mtime;
            utime(tempdestination, &timebuff);
            copyDir(tempsource, tempdestination); //递归操作
        }
        else if (entry->d_type == 10) //d_type为4表示是一个符号连接
        {
            strcpy(tempsource, source);
            strcpy(tempdestination, destination);
            strcat(tempsource, "/");
            strcat(tempsource, entry->d_name);
            strcat(tempdestination, "/");
            strcat(tempdestination, entry->d_name);
            //将源路径和目标路径都复制到temp中
            copyLink(tempsource, tempdestination);
        }
        else
        {
            strcpy(tempsource, source);
            strcpy(tempdestination, destination);
            strcat(tempsource, "/");
            strcat(tempsource, entry->d_name);
            strcat(tempdestination, "/");
            strcat(tempdestination, entry->d_name);
            //将源路径和目标路径都复制到temp中
            copyFile(tempsource, tempdestination);
        }
    }
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("Please enter VALID parameters\n");
        printf("Like: mycp sourceDIR destinationDIR\n");
    }

    DIR *dir;
    if ((dir = opendir(argv[1])) == NULL)
    {
        printf("Can not find the Source Direct");
        exit(0);
    }
    closedir(dir);
    if ((dir = opendir(argv[2])) == NULL)
    {
        struct stat statbuff;
        struct utimbuf timebuff;
        lstat(argv[1], &statbuff);
        //使用lstat可以支持软连接
        mkdir(argv[2], statbuff.st_mode);
        timebuff.actime = statbuff.st_atime;
        timebuff.modtime = statbuff.st_mtime;
        utime(argv[2], &timebuff);
    }
    copyDir(argv[1], argv[2]);
    printf("Copy is ok");
    return 0;
}