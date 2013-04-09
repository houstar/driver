#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define DEVICE_NAME "/dev/led_dev"
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
		while(1){
			ioctl(fd,LED_ON);
			sleep(1);
			ioctl(fd,LED_OFF);
			sleep(1);
		}
		close(fd);
		printf("Close the led device test\n");
	}
	return 0;
}
