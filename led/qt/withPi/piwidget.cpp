#include <QApplication>
#include <QLabel>
#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>
#include <QHBoxLayout>
#include "piwidget.h"

PiWidget::PiWidget(QWidget* parent) : QWidget(parent)
{
    // Layout radiobutton widget
    QLabel* labelLED = new QLabel("LED", this);
    QRadioButton* radioButtonOn = new QRadioButton("0&n", this);
    QRadioButton* radioButtonOff = new QRadioButton("0&n", this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    horizontalLayout->addLayout(radioButtonOn);
    horizontalLayout->addLayout(radioButtonOff);

    connect(radioButtonOn, SIGNAL(clicked()), SLOT(ledOn()));
    connect(radioButtonOff, SIGNAL(clicked()), SLOT(ledOff()));
    setLayout(horizontalLayout);
}

void PiWidget::ledOn()
{
    ledControl(LED, 1);
}

void PiWidget::ledOff()
{
    ledControl(LED, 0);
}
