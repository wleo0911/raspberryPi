#include "gpioled.h"
#include <wiringPi.h>

#define loopCount   10
#define delayTime  250

GpioLED::GpioLED()
{
    // 2. pinmode
    pinMode(LED0, OUTPUT);
    digitalWrite(LED0, LOW);
}

int GpioLED::ledBlink(int gpio)
{
    int i;
    for (i=0;i<loopCount;i++)
    {
        digitalWrite(gpio, HIGH);
        delay(delayTime);
        digitalWrite(gpio, LOW);
        delay(delayTime);
    }
    return 0;
}

int GpioLED::ledCTRL(int gpioPin, int ledCmd)
{
    digitalWrite(gpioPin, ledCmd);
    return 0;
}
