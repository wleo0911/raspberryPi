#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <pthread.h>

int on=250, off=250;
char ch;

void *light(void *arg)
{
	while(ch!='c' && on>0 && off>0)
	{
       	digitalWrite (0, HIGH); 
        usleep(on*10);     //LED on 
        
		digitalWrite (0, LOW); 
        usleep(off*10);    //LED off
    }
}

int main (void)

{
	pthread_t pt;
	wiringPiSetup (); //  wiringPi 초기화 함수
	
	// 1.
	pinMode (0, OUTPUT); // 0 번핀을 출력모드로 쓰겠다 

	// 2.
	pthread_create(&pt, NULL, light, NULL);
	while(ch!='c' && on>0 && off>0)
	{
		scanf("%c", &ch);
		switch (ch)
		{
			case 'a':
			printf("press '%c'\n", ch);
			on = on + 50;
			off = off -50;
			printf("on = %d, off = %d\n", on, off);
			break;

			case 'd':
			printf("press '%c'\n", ch);
			on = on -50;
			off = off + 50;
			printf("on = %d, off = %d\n", on, off);
			break;

			default:
			break;
		}
	}
	// 3.
	pthread_join(pt, NULL);
	return 0;
	
}

