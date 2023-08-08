#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>
#include "NIDAQmx.h"

class Widget : public QWidget
{
	Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
	~Widget();

private slots:
	void device();
	void updateConnectionList();
	void setInterfaceMode(bool capturing);
	void checkReadiness();
	void start();
	void stop();
	void check();
	void read();

private:
    QComboBox *deviceComboBox = new QComboBox();
    QSpinBox *chanalSpinBox = new QSpinBox();
    QComboBox *connectionComboBox = new QComboBox();
    QSpinBox *frequencySpinBox = new QSpinBox();
    QSpinBox *sampleCountSpinBox = new QSpinBox();
    QDoubleSpinBox *valMinDoubleSpinBox = new QDoubleSpinBox();
    QDoubleSpinBox *valMaxDoubleSpinBox = new QDoubleSpinBox();
    QLineEdit *fileNameLineEdit = new QLineEdit();
    QPushButton *startButton = new QPushButton("&START");
    QPushButton *stopButton = new QPushButton("&STOP");
    QProgressBar *valueProgressBar = new QProgressBar();
    QLabel *ReadSaveFaleLabel = new QLabel();

    TaskHandle taskHandle = nullptr;
    QTimer *timer = new QTimer(this);
    QTextStream stream;

	int channelCount;
};
#endif // WIDGET_H
