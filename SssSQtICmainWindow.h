#ifndef SSSSQTICMAINWINDOW_H
#define SSSSQTICMAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QGraphicsScene>

namespace Ui {

class SssSQtICmainWindow;

} // namespace Ui

class SssSQtICmainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit SssSQtICmainWindow(QWidget *parent = 0);
	~SssSQtICmainWindow();

private:
	Ui::SssSQtICmainWindow *pUI;
	QFileSystemModel *pFSM;
	QGraphicsScene *pGS;
	QGraphicsPixmapItem *pGPI;

	void initActions();
	void initGraphicsView();
	void initTreeView();

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void resizeEvent(QResizeEvent *event);
	void showEvent(QShowEvent *event);

private slots:
	void onMenuOpen();
	void onMenuQuit();
	void openPath(const QString &sPath);
	void opened(const QString &newPath);
	void opened2(const QString &path);
	void onClick(QModelIndex index);
	void onDoubleClick(QModelIndex index);

	void on_buttonPrevious_clicked();
	void on_buttonRotateCCW_clicked();
	void on_buttonCrop_clicked();
	void on_buttonRotateCW_clicked();
	void on_buttonNext_clicked();
	void on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint);
};

#endif // SSSSQTICMAINWINDOW_H
