/*
    usage

    gcc -o turnled turnled.c
    sudo ./turnled 17
        sudo : /dev/mem 접근 권한 설정
        17 : 출력모드
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

//Raspi 3 PHYSICAL I/O PERI BASE ADDR : physical momory address
#define BCM_IO_BASE 0x3F000000

//GPIO ADDR(BASE_ADDR + 0x200000) : I/O Peripherals 영역을 접근하기 위한 주소값 설정.
#define GPIO_BASE (BCM_IO_BASE + 0x200000)

// GPIO Function Select 0 ~ 5 [0x3F200000 ~ 0x3F200014]
#define GPIO_IN(g) (*(gpio+((g)/10)) &= ~(7<<(((g)%10)*3)))
#define GPIO_OUT(g) (*(gpio+((g)/10)) |= (1<<(((g)%10)*3)))

//GPIO Pin Output Set 0 / Clr 0
#define GPIO_SET(g) (*(gpio+7) = (1<<g))
#define GPIO_CLR(g) (*(gpio+10) = 1<<g)

#define GPIO_GET(g) (*(gpio+13)&(1<<g))

#define GPIO_SIZE 0xB4

volatile unsigned int *gpio;


int main(int argc, char** argv)
{
	int gno, i, mem_fd;
        void *gpio_map; // 주소값만 받아오고 타입이 없다.
	
	// 핀번호입력이 안됐을때
	if(argc<2)
	{
		printf("Usage : %s GPIO_NO\n", argv[0]);
		return -1;
	}

	gno = atoi(argv[1]);
	
	// device open : /dev/mem 
        // 물리 메모리에서 가상 메모리로 바뀐 주소값 (5pg : Virtual memory)
	if((mem_fd = open("/dev/mem", O_RDWR | O_SYNC))<0)
	{
		printf("open() /dev/mem\n");
		return -1;
	}

        // mmap() -> kernel thread
        gpio_map = mmap(NULL, GPIO_SIZE, PROT_READ | PROT_WRITE,MAP_SHARED, mem_fd, GPIO_BASE); // 물리 번지가 가상 번지로 접근이 된다.

	if(gpio_map==MAP_FAILED)
	{
		printf("[Error] mmap() : %d\n", (int)gpio_map);
		return -1;
	}

        //
	gpio = (volatile unsigned int *)gpio_map;

	GPIO_OUT(gno);

	for(i=0;i<5;i++)
	{
		GPIO_SET(gno);
		sleep(1);
		GPIO_CLR(gno);
		sleep(1);
	}

	munmap(gpio_map, GPIO_SIZE);

	return 0;

}
