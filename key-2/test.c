#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>


#define DEVICE_NAME "/dev/keys"
#define LED_ON 1
#define LED_OFF 0

int main()
{
	int fd;
	char key = 0;
	printf("Start to test the key device\n");
	fd=open(DEVICE_NAME,O_RDWR);
	if(fd == -1)
	{
		printf("Open device file :%s error\n",DEVICE_NAME);
		return 0;
	}
	else
	{
		ioctl(fd,0);
		while(key != 6)
		{
			if(read(fd,&key,1) > 0)
			{
				printf("******key value = %c *********\n",key);
			}
		}
	}
	close(fd);
	printf("Close the led device test\n");
	return 0;
}
