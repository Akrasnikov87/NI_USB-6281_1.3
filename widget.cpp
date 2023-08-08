#include "widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent)
{
	setWindowTitle("NI USB-6281 1.3");
    move(650,200);

    setMaximumWidth(900);
	setMinimumWidth(500);

	setMaximumHeight(200);
	setMinimumHeight(100);

	QHBoxLayout* phbxLayout = new QHBoxLayout;
	phbxLayout->addWidget(deviceComboBox,1);
	phbxLayout->addWidget(chanalSpinBox,1);
	phbxLayout->addWidget(connectionComboBox,1);
	phbxLayout->addWidget(frequencySpinBox,1);
	phbxLayout->addWidget(sampleCountSpinBox,1);

    phbxLayout->addWidget(valMinDoubleSpinBox,1);
    phbxLayout->addWidget(valMaxDoubleSpinBox,1);

	phbxLayout->addWidget(fileNameLineEdit,1);
	phbxLayout->addStretch(3);

	QHBoxLayout* pphbxLayout = new QHBoxLayout;
	pphbxLayout->addWidget(startButton,1);
	pphbxLayout->addWidget(stopButton,1);
	pphbxLayout->addStretch(3);

	QHBoxLayout* ppphbxLayout = new QHBoxLayout;
	ppphbxLayout->addWidget(valueProgressBar);
	ppphbxLayout->addWidget(ReadSaveFaleLabel);

	QVBoxLayout* pvbxLayout = new QVBoxLayout;
	pvbxLayout->addLayout(phbxLayout);
	pvbxLayout->addLayout(pphbxLayout);
	pvbxLayout->addLayout(ppphbxLayout);

	setLayout(pvbxLayout);

	QFile *file = new QFile(this);
	stream.setDevice(file);

	valueProgressBar->setVisible(false);
	startButton->setEnabled(false);
	stopButton->setEnabled(false);

	fileNameLineEdit->setPlaceholderText("file name.txt");

    fileNameLineEdit->setMinimumSize(150,20);
	fileNameLineEdit->setMaximumSize(300,20);

	frequencySpinBox->setValue(1);
	frequencySpinBox->setMaximum(660000);
	frequencySpinBox->setSuffix("  HZ");

	sampleCountSpinBox->setSuffix("  t,c");

	chanalSpinBox->setMaximum(15);
	chanalSpinBox->setSuffix("  channel");

    valMinDoubleSpinBox->setRange(-10,-1);
    valMaxDoubleSpinBox->setRange(1,10);
    valMinDoubleSpinBox->setSuffix("  range min");
    valMaxDoubleSpinBox->setSuffix("  range max");

	connect(deviceComboBox,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&Widget::updateConnectionList);

	connect(deviceComboBox,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&Widget::checkReadiness);
	connect(frequencySpinBox,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),this,&Widget::checkReadiness);
	connect(sampleCountSpinBox,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),this,&Widget::checkReadiness);
	connect(fileNameLineEdit,&QLineEdit::textChanged,this,&Widget::checkReadiness);

	connect(fileNameLineEdit,&QLineEdit::textChanged,file,&QFile::setFileName);
	connect(startButton,&QPushButton::clicked,this,&Widget::start);
	connect(stopButton,&QPushButton::clicked,this,&Widget::stop);

	connect(timer,&QTimer::timeout,this,&Widget::check);

	device();
}
Widget::~Widget()
{
    if (timer->isActive()) timer->stop();
}
void Widget::device()
{
	char deviceArray[1000];

	DAQmxGetSysDevNames(deviceArray,sizeof(deviceArray));

	QStringList deviceList = QString(deviceArray).split(", ",QString::SkipEmptyParts);

    if (!deviceList.isEmpty()) {
		deviceComboBox->setEnabled(true);
		deviceComboBox->addItems(deviceList);
    } else {
		deviceComboBox->setEnabled(false);
		connectionComboBox->setEnabled(false);
	}
}
void Widget::updateConnectionList()
{
	connectionComboBox->addItem("Non-Referenced Single-Ended",DAQmx_Val_NRSE);
	connectionComboBox->addItem("Referenced Single-Ended",DAQmx_Val_RSE);
	connectionComboBox->addItem("Differential",DAQmx_Val_Diff);
}
void Widget::setInterfaceMode(bool capturing)
{
	deviceComboBox->setEnabled(!capturing);
	connectionComboBox->setEnabled(!capturing);
	frequencySpinBox->setEnabled(!capturing);
	sampleCountSpinBox->setEnabled(!capturing);
	fileNameLineEdit->setEnabled(!capturing);
	startButton->setEnabled(!capturing);
	stopButton->setEnabled(capturing);
	valueProgressBar->setVisible(capturing);
}
void Widget::checkReadiness()
{
	startButton->setEnabled(deviceComboBox->currentIndex() != -1 && frequencySpinBox->value() != 0 && sampleCountSpinBox->value() != 0 && !fileNameLineEdit->text().isEmpty());
	sampleCountSpinBox->setMaximum(50000000/frequencySpinBox->value());
}
void Widget::start()
{
	channelCount = 0;

	valueProgressBar->setValue(valueProgressBar->minimum());
	valueProgressBar->setMaximum(sampleCountSpinBox->value() * frequencySpinBox->value());

	setInterfaceMode(true);

	qobject_cast<QFile*>(stream.device())->open(QIODevice::WriteOnly | QIODevice::Text);

	//DAQmx Configure Code
	DAQmxCreateTask("",&taskHandle);

    if (chanalSpinBox->value() == 0 || chanalSpinBox->value() == 1 || chanalSpinBox->value() == 2 || chanalSpinBox->value() == 3 || chanalSpinBox->value() == 4 || chanalSpinBox->value() == 5 || chanalSpinBox->value() == 6 || chanalSpinBox->value() == 7 || chanalSpinBox->value() == 8 || chanalSpinBox->value() == 9 || chanalSpinBox->value() == 10 || chanalSpinBox->value() == 11 || chanalSpinBox->value() == 12 || chanalSpinBox->value() == 13 || chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai0").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 1 || chanalSpinBox->value() == 2 || chanalSpinBox->value() == 3 || chanalSpinBox->value() == 4 || chanalSpinBox->value() == 5 || chanalSpinBox->value() == 6 || chanalSpinBox->value() == 7 || chanalSpinBox->value() == 8 || chanalSpinBox->value() == 9 || chanalSpinBox->value() == 10 || chanalSpinBox->value() == 11 || chanalSpinBox->value() == 12 || chanalSpinBox->value() == 13 || chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai1").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 2 || chanalSpinBox->value() == 3 || chanalSpinBox->value() == 4 || chanalSpinBox->value() == 5 || chanalSpinBox->value() == 6 || chanalSpinBox->value() == 7 || chanalSpinBox->value() == 8 || chanalSpinBox->value() == 9 || chanalSpinBox->value() == 10 || chanalSpinBox->value() == 11 || chanalSpinBox->value() == 12 || chanalSpinBox->value() == 13 || chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai2").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 3 || chanalSpinBox->value() == 4 || chanalSpinBox->value() == 5 || chanalSpinBox->value() == 6 || chanalSpinBox->value() == 7 || chanalSpinBox->value() == 8 || chanalSpinBox->value() == 9 || chanalSpinBox->value() == 10 || chanalSpinBox->value() == 11 || chanalSpinBox->value() == 12 || chanalSpinBox->value() == 13 || chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai3").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 4 || chanalSpinBox->value() == 5 || chanalSpinBox->value() == 6 || chanalSpinBox->value() == 7 || chanalSpinBox->value() == 8 || chanalSpinBox->value() == 9 || chanalSpinBox->value() == 10 || chanalSpinBox->value() == 11 || chanalSpinBox->value() == 12 || chanalSpinBox->value() == 13 || chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai4").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 5 || chanalSpinBox->value() == 6 || chanalSpinBox->value() == 7 || chanalSpinBox->value() == 8 || chanalSpinBox->value() == 9 || chanalSpinBox->value() == 10 || chanalSpinBox->value() == 11 || chanalSpinBox->value() == 12 || chanalSpinBox->value() == 13 || chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai5").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 6 || chanalSpinBox->value() == 7 || chanalSpinBox->value() == 8 || chanalSpinBox->value() == 9 || chanalSpinBox->value() == 10 || chanalSpinBox->value() == 11 || chanalSpinBox->value() == 12 || chanalSpinBox->value() == 13 || chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai6").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 7 || chanalSpinBox->value() == 8 || chanalSpinBox->value() == 9 || chanalSpinBox->value() == 10 || chanalSpinBox->value() == 11 || chanalSpinBox->value() == 12 || chanalSpinBox->value() == 13 || chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai7").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 8 || chanalSpinBox->value() == 9 || chanalSpinBox->value() == 10 || chanalSpinBox->value() == 11 || chanalSpinBox->value() == 12 || chanalSpinBox->value() == 13 || chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai8").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 9 || chanalSpinBox->value() == 10 || chanalSpinBox->value() == 11 || chanalSpinBox->value() == 12 || chanalSpinBox->value() == 13 || chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai9").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 10 || chanalSpinBox->value() == 11 || chanalSpinBox->value() == 12 || chanalSpinBox->value() == 13 || chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai10").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 11 || chanalSpinBox->value() == 12 || chanalSpinBox->value() == 13 || chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai11").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 12 || chanalSpinBox->value() == 13 || chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai12").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 13 || chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai13").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 14 || chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai14").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}
    if (chanalSpinBox->value() == 15) {
        DAQmxCreateAIVoltageChan(taskHandle,QString("%1/%2").arg((deviceComboBox->currentText()).toStdString().data()).arg("ai15").toUtf8(),"",connectionComboBox->currentData().toInt(),valMinDoubleSpinBox->value(),valMaxDoubleSpinBox->value(),DAQmx_Val_Volts,nullptr);
		channelCount++;
	}

    DAQmxCfgSampClkTiming(taskHandle,"",frequencySpinBox->value(),DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,unsigned(sampleCountSpinBox->value()) * unsigned(frequencySpinBox->value()));

	//DAQmx Start Code
	DAQmxStartTask(taskHandle);

	timer->start();
	ReadSaveFaleLabel->setText("READ..");
}
void Widget::stop()
{
	timer->stop();

	//DAQmx Stop Code
	DAQmxStopTask(taskHandle);
	DAQmxClearTask(taskHandle);

	taskHandle = nullptr;

	qobject_cast<QFile*>(stream.device())->close();

	setInterfaceMode(false);

	ReadSaveFaleLabel->clear();
}
void Widget::check()
{
    uInt32 capturedSampleCount = 0L;
	DAQmxGetReadAvailSampPerChan(taskHandle,&capturedSampleCount);

    valueProgressBar->setValue(int(capturedSampleCount));
    if (valueProgressBar->value() == valueProgressBar->maximum()) {
		timer->stop();
		read();
	}
}
void Widget::read()
{
	int n = 0;
    int32 read;
	double *data = new double[sampleCountSpinBox->value() * frequencySpinBox->value()];

	//DAQmx Read Code
    DAQmxReadAnalogF64(taskHandle,DAQmx_Val_Auto,DAQmx_Val_WaitInfinitely,DAQmx_Val_GroupByScanNumber,data,unsigned(sampleCountSpinBox->value()) * unsigned(frequencySpinBox->value()),&read,nullptr);

	valueProgressBar->setMaximum(read);
	ReadSaveFaleLabel->setText("SAVE FILE..");

	double cout = 0.0;

    for (int sampleNumber = 0; sampleNumber < read; ++sampleNumber) {
		//stream << QString::number((sampleNumber),'f',0).append("  ");

        if (sampleNumber == n) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 1) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 1) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 2) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 2) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 3) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 3) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 4) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 4) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 5) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 5) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 6){
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 6) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 7) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 7) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 8) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 8) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 9) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 9) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 10) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 10) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 11) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 11) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 12) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 12) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 13) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 13) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 14) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 14) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 15) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}
        if (sampleNumber == n + 15) {
			stream << QString("%1 %2").arg(data[sampleNumber],0,'f',10).arg((cout++/frequencySpinBox->value())*channelCount).append("  ");
            if (channelCount == 16) {
				n = sampleNumber + 1;
				stream << "\n";
			}
		}

		valueProgressBar->setValue(sampleNumber);

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
	}
	delete [] data;
	stop();
}
