#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

void main()
{
	int fd;
	int counter=0;
	int old_counter=0;
	fd=open("/dev/second",O_RDONLY);
	if(fd!=1){
		while(1){
			read(fd,&counter,sizeof(unsigned int));
			if(counter!=old_counter){
				printf("seconds after open /dev/second :%d\n",counter);
				old_counter=counter;
			}
		}
	}else{
		printf("Device open failure\n");
	}
	
}
