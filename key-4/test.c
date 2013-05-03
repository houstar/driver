#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>


#define DEVICE_NAME "/dev/keys"
#define LED_ON 1
#define LED_OFF 0

int main()
{
	int fd;
	char buf = 0;
	int ret;
	int len;
	struct pollfd fds[1];
	printf("Start to test the key device\n");
	fd=open(DEVICE_NAME,O_RDWR);
	if(fd == -1)
	{
		printf("Open device file :%s error\n",DEVICE_NAME);
		return 0;
	}
	while(1){
		fds[0].fd = fd;
		fds[0].events = POLLIN;
		ret = poll(fds,1,5000);
		ioctl(fd,0);
		if(ret == 0)
		{
			printf("time out\n");
		}
		else if( ret > 0  && (read(fd,&buf,1) >0))
		{
			printf("key = %c\n",buf);
		}
		else {
			printf("error\n");
		}
	}
	close(fd);
	printf("Close the led device test\n");
	return 0;
}
