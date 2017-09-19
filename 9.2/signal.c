#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#define MAX_LEN 100
void input_handle(int num)
{
	char data[MAX_LEN];
	int len;
	len=read(STDIN_FILENO,&data,MAX_LEN);
	data[len]=0;
	printf("input available:%s\n",data);
}
int main(void)
{
	int oflag;
	signal(SIGIO,input_handle);
	fcntl(STDIN_FILENO,F_SETOWN,getpid());
	oflag=fcntl(STDIN_FILENO,F_GETFL);
	fcntl(STDIN_FILENO,F_SETFL,oflag|FASYNC);
	while(1);
}
