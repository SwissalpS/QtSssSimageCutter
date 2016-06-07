#include "SssSQtICmainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {

	QApplication oApp(argc, argv);
	SssSQtICmainWindow oMainWindow;

	oMainWindow.show();

	return oApp.exec();

} // main
