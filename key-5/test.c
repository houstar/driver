#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>


#define DEVICE_NAME "/dev/keys"
int fd;

void signal_function(int sig)
{
	char key;
	while(read(fd,&key,1) ==1)
	{
		printf("key = 0x%x\n",key);
	}
}

int main()
{
	char buf = 0;
	int ret;
	int len;
	long flag;
	printf("Start to test the key device\n");
	fd=open(DEVICE_NAME,O_RDWR);
	if(fd == -1)
	{
		printf("Open device file :%s error\n",DEVICE_NAME);
		return 0;
	}
	signal(SIGIO,signal_function);

	while(1){
	}
	close(fd);
	printf("Close the led device test\n");
	return 0;
}
