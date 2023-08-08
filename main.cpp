#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Widget National_Instruments;
	National_Instruments.show();

	return a.exec();
}
