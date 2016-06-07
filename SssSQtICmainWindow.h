#ifndef SSSSQTICMAINWINDOW_H
#define SSSSQTICMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class SssSQtICmainWindow;
}

class SssSQtICmainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit SssSQtICmainWindow(QWidget *parent = 0);
	~SssSQtICmainWindow();

private:
	Ui::SssSQtICmainWindow *ui;
};

#endif // SSSSQTICMAINWINDOW_H
