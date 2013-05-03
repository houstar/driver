#ifndef LEDDIALOG_H
#define LEDDIALOG_H

#include <QApplication>
#include <QDialog>

#include "ui_leddialog.h"

class LEDDialog : public QDialog , public Ui::LEDDialog
{
	Q_OBJECT
	public:
		LEDDialog(QWidget *parent = 0);
		~LEDDialog();
	private slots:
		/* slotConnectByObjectName */
		void on_ledOneCheckBox_stateChanged(int state);
		void on_ledTwoCheckBox_stateChanged(int state);
		void on_ledThreeCheckBox_stateChanged(int state);
		void on_ledFourCheckBox_stateChanged(int state);

		void allOffCheckBoxClicked();
		void allOnCheckBoxClicked();

	private:
		/* File Descriptions */
		int fd;

        void OpenDevice();
};
#endif // LEDDIALOG_H

