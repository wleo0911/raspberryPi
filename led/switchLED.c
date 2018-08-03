// gcc -o name name.c -lwiringPi

#include <wiringPi.h>

#define LED0 0 // switch가 안 눌린 상태
#define SW   1 // switch가 눌린 상태 

int main(void)
{
	int swValue;
	int flag;

	// 1. wiringPi Init
	wiringPiSetup();

	// 2. pinMode Init
	pinMode(SW,INPUT);  //Physical 12, WPI 1, GPIO 18을 입력모드로, switch 눌림(1)
											//& gpio readall로 눌렀을 때(1)와 누르지 않았을 때(0) 값을 비교할 수 있다. 

											// 보드의 physical number -> GPIO.setmode(GPIO.BOARD)
											// 보드의 GPIO 핀 번호 -> GPIO.setmode(GPIO.BCM)

	pinMode(LED0,OUTPUT);//Physical 11, WPI 0, GPIO 17을 출력모드로, switch 안 눌림(0)

	// 3. read/write 
	while(1)
	{
		swValue = digitalRead(SW); // 입력모드에서 switch의 상태에 따른 값을 가져온다.		
		if(swValue)
			flag = ~flag; // swValue 값이 1이(switch가 눌린 상태)라면 flag를 참이면 거짓으로 거짓이면 참으로

		if(flag)
			digitalWrite(LED0, HIGH); // flag가 참일 때 출력모드에 LED on을 입력한다.
		else
			digitalWrite(LED0, LOW);  // flag가 거짓일 때 출력모드에 LED off를 입력한다.
		delay(100);
	}
	return 0;

