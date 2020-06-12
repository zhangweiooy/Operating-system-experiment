#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main(int argc,char *argv[])
{
	printf("Run Program2\n");
	sleep(atoi(argv[2]));
	return 0;
}
