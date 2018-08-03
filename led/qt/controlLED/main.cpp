#include "mainwindow.h"
#include <QApplication>
#include <wiringPi.h>
#include "gpioled.h"



int main(int argc, char *argv[])
{
    // 1. wiringpit Init
    wiringPiSetup();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
