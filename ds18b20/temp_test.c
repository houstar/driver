#include "stdio.h"
#include "sys/types.h"
#include "sys/ioctl.h"
#include "stdlib.h"
#include "termios.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "sys/time.h"

main()
{
    int fd;

    unsigned char buf[2];


    if ((fd=open("/dev/Tem",O_RDWR | O_NDELAY | O_NOCTTY)) < 0)
    {
        printf("Open Device DS18B20 failed.\r\n");
        exit(1);
    }
    else
    {
        printf("Open Device DS18B20 successed.\r\n");
        while(1)
        {
            read(fd, buf, sizeof(buf));

            printf("%d.%dC\r\n", buf[0], buf[1]);
            sleep(1);

        }
        close(fd);
    }
}
