#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define DEVICE_NAME "/dev/buzzer"

int main()
{
	int fd;
	printf("Start to test the buzzer device\n");
	fd=open(DEVICE_NAME,O_RDWR);
	if(fd==-1)
		printf("Open device file :%s error\n",DEVICE_NAME);
	else{
		while(1)
		{
			ioctl(fd,1);
			sleep(1);
			ioctl(fd,0);
			sleep(1);
		}
		close(fd);
		printf("Close the led device test\n");
	}
	return 0;
}
