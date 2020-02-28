#include "src/EDrawCommands.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	EDrawCommands w;
	w.setMinimumWidth(512);
	w.setMinimumHeight(512);
	w.show();
	return a.exec();
}
