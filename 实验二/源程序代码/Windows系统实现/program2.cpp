#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
int main(int argc,char *argv[])
{
    printf("Run program2\n");printf("%s\n",argv[2]);
    int time=atoi(argv[1]);
    Sleep(1000*time);
    return 0;
}
