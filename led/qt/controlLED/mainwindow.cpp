#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <wiringPi.h>
#include "gpioled.h"
#include <qstring.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnLEDON_clicked()
{
    GpioLED gpioLED1;
    gpioLED1.ledCTRL(LED0,HIGH);
    QString strOperation, strLedStatus;
    strOperation.sprintf("LED Control");
    strLedStatus.sprintf("LED ON");
    ui->labelOp->setText(strOperation);
    ui->labelStatus->setText(strLedStatus);
}

void MainWindow::on_btnLEDOFF_clicked()
{
    GpioLED gpioLED2;www
    gpioLED2.ledCTRL(LED0,LOW);
    QString strOperation, strLedStatus;
    strOperation.sprintf("LED Control");
    strLedStatus.sprintf("LED OFF");
    ui->labelOp->setText(strOperation);
    ui->labelStatus->setText(strLedStatus);
}
