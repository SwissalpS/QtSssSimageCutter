#include "SssSQtICmainWindow.h"
#include "ui_SssSQtICmainWindow.h"

SssSQtICmainWindow::SssSQtICmainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::SssSQtICmainWindow) {

	ui->setupUi(this);

} // __constructor

SssSQtICmainWindow::~SssSQtICmainWindow() {

	delete ui;

} // __destructor
