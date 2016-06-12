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
	QImage *pCurrentImage;
	QString *sPathFileCurrent;
	QRectF *pRubberSelection;
	QModelIndex oCurrentSelectedIndex;
	QPen oPenCropLines;
	QPen oPenCropBoxOutline;
	QBrush oBrushCropBoxFill;
	QGraphicsLineItem *pGLBottom;
	QGraphicsLineItem *pGLLeft;
	QGraphicsLineItem *pGLRight;
	QGraphicsLineItem *pGLTop;
	QGraphicsRectItem *pGRBottom;
	QGraphicsRectItem *pGRLeft;
	QGraphicsRectItem *pGRRight;
	QGraphicsRectItem *pGRTop;
	QGraphicsPixmapItem *pGPI;

	bool bShowingCrop;
	bool bImageChanged;

	void initActions();
	void initGraphicsView();
	void initTreeView();

	QRectF rubberRectNormalized();

protected:
	void rotateImage(qreal fAngle);
	void rubberReleased();
	void saveImage();
	void saveAndDestroyImage();
	void updateGraphicsView();
	void updateLandscapeIndicator();
	void updatePixmap();

	// events
	void contextMenuEvent(QContextMenuEvent *event);
	void resizeEvent(QResizeEvent *event);

private slots:
	void onMenuOpen();
	void onMenuQuit();
	void openPath(const QString &sPath);

	void opened(const QString &newPath);
	void opened2(const QString &path);
	void onClick(QModelIndex index);
	void onTreeSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
	void onDoubleClick(QModelIndex index);

	void on_buttonPrevious_clicked();
	void on_buttonRotateCCW_clicked();
	void on_buttonCrop_clicked();
	void on_buttonRotateCW_clicked();
	void on_buttonNext_clicked();
	void on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint);

	void on_checkBoxExpandAll_stateChanged(int iState);

public slots:
	void loadImage(const QString &sPathFile);
	void removeCropMarker();

};

#endif // SSSSQTICMAINWINDOW_H
