#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#define BUFFER_LEN 20
#define FIFO_CLEAR 0x01
void main()
{
	int fd,num;
	char rd_ch[BUFFER_LEN];
	fd_set rfds,wfds;
	fd=open("/dev/globalfifo",O_RDONLY|O_NONBLOCK);
	if(fd!=-1)
	{
		if(ioctl(fd,FIFO_CLEAR,0)<0)
			printf("ioctl command faied\n");
		while(1)
		{
			FD_ZERO(&rfds);
			FD_ZERO(&wfds);
			FD_SET(fd,&rfds);
			FD_SET(fd,&wfds);
			select(fd+1,&rfds,&wfds,NULL,NULL);
			if(FD_ISSET(fd,&rfds))
				printf("Poll monitor:can be read!\n");
			if(FD_ISSET(fd,&wfds))
				printf("Poll monitor:can be written\n");
		}	
	}else{
		printf("Device open failure\n");
	}
	
}
