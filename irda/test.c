#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>


#define DEVICE_NAME "/dev/irda0"

int main()
{
	int fd;
	char buf = 0;
	printf("Start to test the irda device\n");
	fd=open(DEVICE_NAME,O_RDWR);
	if(fd == -1)
	{
		printf("Open device file :%s error\n",DEVICE_NAME);
		return 0;
	}
	else
	{
		while(1)
		{
			read(fd,&buf,1);
			if(buf == 0)
				printf("0\n");
			else
				printf("1\n");
			sleep(1);
		}
	}
	close(fd);
	printf("Close the irda device test\n");
	return 0;
}
