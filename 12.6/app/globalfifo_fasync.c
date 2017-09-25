#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
int fd;
static void signalio_handle(int signum)
{
	char data[100];
	int len;
	printf("recieve a signal from globalfifo,signalnum:%d\n",signum);
	len=read(fd,&data,100);
	data[len]='\0';
	printf("input available:%s",data);
}
void main()
{
	int oflags;
	fd=open("/dev/globalfifo",O_RDWR,S_IRUSR|S_IWUSR);
	if(fd!=-1)
	{
		signal(SIGIO,signalio_handle);
		fcntl(fd,F_SETOWN,getpid());
		oflags=fcntl(fd,F_GETFL);
		fcntl(fd,F_SETFL,oflags|FASYNC);
		while(1){
			sleep(100);
		}	
	}else{
		printf("device open failure\n");
	}
		
}
