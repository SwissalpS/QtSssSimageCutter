#ifndef QTSSSSICMAINWINDOW_H
#define QTSSSSICMAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QGraphicsScene>



namespace Ui {



class QtSssSiCmainWindow;



} // namespace Ui



namespace SwissalpS { namespace QtSssSiC {



class QtSssSiCcropLine;
class QtSssSiCmainWindow : public QMainWindow {

	Q_OBJECT

public:
	explicit QtSssSiCmainWindow(QWidget *parent = 0);
	~QtSssSiCmainWindow();

private:
	Ui::QtSssSiCmainWindow *pUI;
	QFileSystemModel *pFSM;
	QGraphicsScene *pGS;
	QImage *pCurrentImage;
	QString *sPathFileCurrent;
	QRectF *pRubberSelection;
	QModelIndex oCurrentSelectedIndex;
	QPen oPenCropLines;
	QPen oPenCropBoxOutline;
	QBrush oBrushCropBoxFill;
	QtSssSiCcropLine *pCLBottom;
	QtSssSiCcropLine *pCLLeft;
	QtSssSiCcropLine *pCLRight;
	QtSssSiCcropLine *pCLTop;
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
	void updateCropBoxes();
	void updateCropLines();
	void updateGraphicsView();
	void updateLandscapeIndicator();
	void updatePixmap();
	void updateStatusMessage();

	// events
	void contextMenuEvent(QContextMenuEvent *event);
	void resizeEvent(QResizeEvent *event);

private slots:
	void onMenuOpen();
	void onMenuQuit();
	void openPath(const QString &sPath);

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

	void on_actionDelete_triggered();

public slots:
	void cropLineMoved();
	void hideCrop();
	void loadImage(const QString &sPathFile);
	void showCrop();

};



}	} // namespace SwissalpS::QtSssSiC



#endif // QTSSSSICMAINWINDOW_H
