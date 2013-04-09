#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEVICE_NAME "/dev/led"


int main(void)
{
	int fd;
	printf("Start go test the led device\n");
	char buf[10]={0,1};
	fd = open(DEVICE_NAME,O_RDWR);
	if(fd < 0)
	{
		printf("Open device file: %s error\n",DEVICE_NAME);
	}
	else
	{
		while(1)
		{
			write(fd,&buf[0],1);
			sleep(1);
			write(fd,&buf[1],1);
			sleep(1);
		}
	}
	close(fd);
	return 0;
}
