#ifndef GPIOLED_H
#define GPIOLED_H

#define LED0 0

class GpioLED
{
public:
    GpioLED();
    int ledBlink(int gpio);
    int ledCTRL(int gpioPin, int ledCmd);
};

#endif // GPIOLED_H
