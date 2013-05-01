#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define DEVICE_NAME "/dev/leds"
#define LED_ON 1
#define LED_OFF 0

int main()
{
	int fd;
	printf("Start to test the led device\n");
	fd=open(DEVICE_NAME,O_RDWR);
	if(fd==-1)
		printf("Open device file :%s error\n",DEVICE_NAME);
	else{
		int i=0;
		while(1)
		{
			if(i>4) i=0;
			ioctl(fd,1,i);
			sleep(1);
			ioctl(fd,0,i);
			sleep(1);
			i++;
		}
		close(fd);
		printf("Close the led device test\n");
	}
	return 0;
}
