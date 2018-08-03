#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#define BUFSIZE 	50

/*
int fin;
pthread_t p_thread[2];
static int temp1=0;
static int temp2=1;

void* add_func(void* data)
{
	while(1)
	{
		sleep(950);
		printf("%d", temp1);
		temp1++;
	}
}
*/

int fd;
char buf[BUFSIZE];
int button; 
pthread_t pt;

static int start_time = 0;
static int cnt = 1;

static int check = 1;
static int delete = 0;

void* timer(void *arg)
{
	while(1)
	{
		printf("%d\n", start_time);
		start_time++;
		sleep(1000);
	}
}


void signal_handler2(int signum)
{
	static int count=0;
	
	printf("user app : signal is catched\n");
	if(signum==SIGIO)          // 신호 반환 시작 
	{
		read(fd,buf,5);
		button = atoi(buf);
		//printf(" buf = %s, button = %d\n", buf, button);
		
		switch(button)
		{
			case 1:  // 빨강
				printf("%d is pressed\n", button);
				if (check)
				{
					pthread_create(&pt, NULL, timer, NULL);
					delete = 0;
					check != check;
				}
				else 
				{
					printf("%d랩 : %d\n", cnt, start_time);
					delete = 0;
				}
				break;
			case 2:  // 파랑
				printf("%d is pressed\n", button);
				if (delete == 1)
				{
					pthread_cancel(pt);
					delete++;
					check != check;
				}
				else if (delete == 2)
				{
					exit(1);	
				}				
				break;
			default:
				break;
		}
	}
		
	count++;
	if(count==10) exit(1);
}

int main(int argc, char** argv)
{

	char i=0;
	int count;
	
	memset(buf, 0, BUFSIZE);
	signal(SIGIO, signal_handler2);
	
	printf("GPIO Set : %s\n", argv[1]);

	// 1. open() 테스트 -> gpio_open()
	fd=open("/dev/gpioled", O_RDWR);
	if(fd<0)
	{
		printf("Error : open()\n");
		return -1;
	}
	
	sprintf(buf,"%s:%d", argv[1], getpid());
	
	// 2. write() 테스 트-> gpio_write()
	count = write(fd, buf, strlen(buf));
	if(count<0)
			printf("Error : write()\n");
			
	// 3. read() 테스트 -> gpio_read()
	count = read(fd, buf, strlen(argv[1]));
	printf("Read data : %s\n", buf);

	while(1);
	
	// 4. close() 테스트 -> gpio_close()
	close(fd);		
	
	return 0;
}
