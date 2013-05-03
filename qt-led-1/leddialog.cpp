#include <QtGui>

#include "leddialog.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#define DEVICE_NAME "/dev/leds"
LEDDialog::LEDDialog(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);

    OpenDevice();

    connect(allOffCheckBox,SIGNAL(clicked()),this,SLOT(allOffCheckBoxClicked()));
    connect(allOnCheckBox,SIGNAL(clicked()),this,SLOT(allOnCheckBoxClicked()));


}
LEDDialog::~LEDDialog()
{
    /* if we don't use :: namespace,something goes wrong
     * 'cause i haven't  time to learn QFile
     */
    ::close(fd);
    delete this;
}

void LEDDialog::on_ledOneCheckBox_stateChanged(int)
{
    ioctl(fd,int(ledOneCheckBox->isChecked()),0);
}

void LEDDialog::on_ledTwoCheckBox_stateChanged(int)
{
    ioctl(fd,int(ledTwoCheckBox->isChecked()),1);
}

void LEDDialog::on_ledThreeCheckBox_stateChanged(int)
{
    ioctl(fd,int(ledThreeCheckBox->isChecked()),2);
}

void LEDDialog::on_ledFourCheckBox_stateChanged(int)
{
    ioctl(fd,int(ledFourCheckBox->isChecked()),3);
}

void LEDDialog::allOffCheckBoxClicked()
{
    ledOneCheckBox->setCheckState(Qt::Unchecked);
    ledTwoCheckBox->setCheckState(Qt::Unchecked);
    ledThreeCheckBox->setCheckState(Qt::Unchecked);
    ledFourCheckBox->setCheckState(Qt::Unchecked);
    allOnCheckBox->setCheckState(Qt::Unchecked);
}
void LEDDialog::allOnCheckBoxClicked()
{
    ledOneCheckBox->setCheckState(Qt::Checked);
    ledTwoCheckBox->setCheckState(Qt::Checked);
    ledThreeCheckBox->setCheckState(Qt::Checked);
    ledFourCheckBox->setCheckState(Qt::Checked);
    allOffCheckBox->setCheckState(Qt::Unchecked);
}

void LEDDialog::OpenDevice()
{
    fd = ::open(DEVICE_NAME,O_RDWR);
    if(fd == -1){
        int ret = QMessageBox::critical(this, tr("LED Open Error"),
                                        tr("An orrr when open LED.\n"
                                           "Close this Window & Exit?"),
                                        QMessageBox::Ok);
        if(ret == QMessageBox::Ok){
            this->close();
        }


    }
}
