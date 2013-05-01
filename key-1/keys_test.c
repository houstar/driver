#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ioctl.h>

int main(int argc,char **argv){
	int i;
    int ret;
  	int fd;
	int press_cnt[6];
	fd=open("/dev/keyint",0);
	if(fd<0)
	printf("打开失败");
	while(1){
	ret=read(fd,press_cnt,sizeof(press_cnt));
	if(ret<0)
	{printf("错误");
	return -1;
    }
 	for(i=0;i<6;i++)
	if(press_cnt[i])
	printf("key%d has been pressed %d \n",(i+1),press_cnt[i]);
   	}
    close(fd);
    return 0;

}












