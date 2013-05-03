#include <QApplication>
#include <QTranslator>

#include "leddialog.h"
int main(int argc,char *argv[])
{
    QApplication app(argc,argv);

	QTranslator trans;
	trans.load("zh_CN.qm");
	app.installTranslator(&trans);

    LEDDialog *leddialog = new LEDDialog;
    leddialog->show();

    return app.exec();
}
