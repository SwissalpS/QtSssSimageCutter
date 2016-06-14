#include "QtSssSiCmainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {

	QApplication oApp(argc, argv);
	SwissalpS::QtSssSiC::QtSssSiCmainWindow oMainWindow;

	oMainWindow.show();

	return oApp.exec();

} // main
