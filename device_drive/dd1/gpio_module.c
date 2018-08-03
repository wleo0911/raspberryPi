/*
 module이란?       device drive에서 작동하는 kernel thread
 main은 왜 없어?    계속 돌아야 하기 때문에!

 make
 sudo insmod gpio_module.ko      -> 모듈 올리기
 lsmod                           -> 모듈 확인
 dmesg
 sudo mknod /dev/gpioled c 200 0 -> gpio_module에 있는 정보를 참조하도록 만들어줌
 sudo chmod 666 /dev/gpioled     ->
 ./gpio 1                        -> 1입력
 ./gpio 0											 ->  0입력
 sudo rmmod gpio_module          -> 모듈 내리기
*/

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HJ PARK");
MODULE_DESCRIPTION("RASPBERRY PI GPIO LED DRIVER");

#define BCM_IO_BASE 0x3F000000					  // Physical addr   
#define GPIO_BASE (BCM_IO_BASE + 0x200000)  // Linux여서 SWAP(Physic -> Vertual)하는 과정에서 메모리 변화 
#define GPIO_IN(g) (*(gpio + ((g) / 10)) &= ~(7 << (((g) % 10) * 3)))  // GPIO Function Select 0~5 [0x3F200000 ~ 0x3F200014] : unsigned int -> 4byte씩 주소값 증가
#define GPIO_OUT(g) (*(gpio + ((g) / 10)) |= (1 << (((g) % 10) * 3)))	  // (설정 레지스터 번지) (설정 비트 위치)
#define GPIO_SET(g) (*(gpio + 7) = (1 << g))  // output set 0 : no effect
#define GPIO_CLR(g) (*(gpio + 10) = 1 << g)  // output clr 0 : no effect
#define GPIO_GET(g) (*(gpio + 13) & (1 << g))
#define GPIO_SIZE 0xB4  //
#define GPIO_MAJOR 200  // crwxrwxrwx ... ... ... 200, 0 
#define GPIO_MINOR 0		 // net으로 kernel에 접근할 때 major number를 사용
#define GPIO_DEVICE "gpioled"
#define GPIO_LED		17	// Raspi GPIO-17 LED
#define STR_SIZE 100

static char msg[STR_SIZE] = {0};  // 중괄호 초기화

struct cdev gpio_cdev;

// 함수원형 선언 
static int gpio_open(struct inode *, struct file *);
static int gpio_close(struct inode *, struct file *);
static ssize_t gpio_read(struct file*, char *, size_t, loff_t *);
static ssize_t gpio_write(struct file*, const char *, size_t, loff_t *);

static struct file_operations gpio_fops = {
		.owner = THIS_MODULE,
		.read   = gpio_read,
		.write = gpio_write,
		.open   = gpio_open,
		.release = gpio_close,
};

volatile unsigned int *gpio;

static int gpio_open(struct inode *inod, struct file *fil)
{
	// app에서 open()가 호출될 때마다 모듈의 사용 카운터를 증가 시킨다.
	try_module_get(THIS_MODULE);
	printk(KERN_INFO "GPIO Device opened\n");
	return 0;
	}

static int gpio_close(struct inode *inod, struct file *fil)
{
	// app에서 close()함수가 호출될 때마다 모듈의 사용 카운터를 감소 시킨다.
	module_put(THIS_MODULE);
	printk(KERN_INFO "GPIO Device closed\n");
	return 0;
}

static ssize_t gpio_read(struct file *inode, char *buff, size_t len, loff_t *off)
{
	// app에서 read()함수가 호출될 때마다 gpio_read()함수가 호출된다.
	int count;
	strcat(msg, "from kernel");
	count = copy_to_user(buff, msg, strlen(msg)+1);

	printk(KERN_INFO "GPIO Device read:%s\n", msg);
	return (ssize_t)count;
}

static ssize_t gpio_write(struct file *inode, const char *buff, size_t len, loff_t *off)
{
	int count;
	memset(msg, 0, STR_SIZE);
	count = copy_from_user(msg,buff,len); // copy buff of kernel`s level to buff of app`s level
	(!strcmp(msg,"0"))?GPIO_CLR(GPIO_LED):GPIO_SET(GPIO_LED);
	printk(KERN_INFO "GPIO Device write : $s\n", msg);
	
	return (ssize_t) count;
}

int initModule(void)
{
	dev_t devno;
	unsigned int count;
	static void *map;
	
	int err;
	
	printk(KERN_INFO "initModule : gpio module init\n");
	
	// 1. 문자 디바이스를 등록한다.
	devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);
	printk(KERN_INFO "devno=%x\n",devno); 
	register_chrdev_region(devno, 1, GPIO_DEVICE);
	
	// 2. 문자 디바이스를 위한 구조체를 초기화 한다.	
	cdev_init(&gpio_cdev, &gpio_fops);
	gpio_cdev.owner = THIS_MODULE;
	count =1;
	
	// 3. 문자디바이스 추가
	err =cdev_add(&gpio_cdev, devno, count);
	if(err<0)
	{
		printk(KERN_INFO "Error : cdev_add()\n");
		return -1;
	}
	
	printk(KERN_INFO "'mknod /dev/%s c %d 0'\n", GPIO_DEVICE, GPIO_MAJOR);
	printk(KERN_INFO "'chmod  666 /dev/%s'\n", GPIO_DEVICE);
	
	// 4. 물리 메모리 번지를 인자로 전달하면 가상 메모리 번지를 리턴한다.
	map = ioremap(GPIO_BASE, GPIO_SIZE);
	if(!map)
	{
		printk(KERN_INFO "Error : mapping GPIO memory\n");
		iounmap(map);
		return -EBUSY;
	}
	
	gpio = (volatile unsigned int*)map; 
	GPIO_OUT(GPIO_LED);     // 위에서 #define으로 17번으로 설정
	
 	return 0;
}

void cleanupModule(void)
{
	dev_t devno;
	devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);
	
	// 1. 문자 디바이스의 등록을 해제한다.
	unregister_chrdev_region(devno, 1);
	
	// 2. 문자 디바이스의 구조체를 삭제한다.
	cdev_del(&gpio_cdev);
	
	// 3. GPIO unmap;
	if(gpio)
		iounmap(gpio);
		
	printk(KERN_INFO "operation is done! : cleanupModule()\n");		
}


module_init(initModule);      // insmod 할 때 호출
module_exit(cleanupModule);  // rmmod 할 때 호출


