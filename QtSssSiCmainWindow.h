#ifndef QTSSSSICMAINWINDOW_H
#define QTSSSSICMAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QGraphicsScene>



namespace Ui {



class QtSssSiCmainWindow;



} // namespace Ui



namespace SwissalpS { namespace QtSssSiC {



class QtSssSiCcropCorner;
class QtSssSiCcropLine;
class QtSssSiCmainWindow : public QMainWindow {

	Q_OBJECT

public:
	explicit QtSssSiCmainWindow(QWidget *parent = 0);
	~QtSssSiCmainWindow();

private:
	// don't reorder members without reordering in constructor!
	Ui::QtSssSiCmainWindow *pUI;
	QFileSystemModel *pFSM;
	QGraphicsScene *pGS;
	QImage *pCurrentImage;
	QString *sPathFileCurrent;
	QRectF *pRubberSelection;
	QModelIndex oCurrentSelectedIndex;
	QPen oPenCropBoxOutline;
	QBrush oBrushCropBoxFill;
	QtSssSiCcropCorner *pCCBottomLeft;
	QtSssSiCcropCorner *pCCBottomRight;
	QtSssSiCcropCorner *pCCTopLeft;
	QtSssSiCcropCorner *pCCTopRight;
	QtSssSiCcropLine *pCLBottom;
	QtSssSiCcropLine *pCLLeft;
	QtSssSiCcropLine *pCLRight;
	QtSssSiCcropLine *pCLTop;
	QGraphicsRectItem *pGRBottom;
	QGraphicsRectItem *pGRLeft;
	QGraphicsRectItem *pGRRight;
	QGraphicsRectItem *pGRTop;
	QGraphicsPixmapItem *pGPI;

	// flags
	bool bShowingCrop;
	bool bImageChanged;

	// private methods, maybe move to protected
	void initActions();
	void initGraphicsView();
	void initTreeView();

protected:
	// have only tested -/+90
	void rotateImage(qreal fAngle);
	// call this rather than accessing pRubberSelection directly
	QRectF rubberRectNormalized();
	// called on-mouse-up
	void rubberReleased();
	// calls saveImage() and destroys pCurrentImage
	void saveAndDestroyImage();
	void saveImage();
	// called by various crop-marking-methods
	void setSelection(QRectF &oNewRect);
	// update positions of handles to rubberRectNormalized()
	void updateCropBoxes();
	void updateCropCorners();
	void updateCropLines();
	// fit-in-view
	void updateGraphicsView();

	void updateLandscapeIndicator();
	void updatePixmap();
	void updateStatusMessage();

	// events
	void contextMenuEvent(QContextMenuEvent *event);
	void resizeEvent(QResizeEvent *event);

private slots:
	// connected in init...
	void onClick(QModelIndex index); // onTreeClick
	void onDoubleClick(QModelIndex index); //onTreeDC
	void onMenuOpen();
	void onMenuQuit();
	void onTreeSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

	// Qt Design triggers, don not rename! (bothways?)
	void on_actionDelete_triggered();
	void on_buttonCrop_clicked();
	void on_buttonNext_clicked();
	void on_buttonPrevious_clicked();
	void on_buttonRotateCCW_clicked();
	void on_buttonRotateCW_clicked();
	void on_checkBoxExpandAll_stateChanged(int iState);
	void on_graphicsView_rubberBandChanged(const QRect &viewportRect,
										   const QPointF &fromScenePoint,
										   const QPointF &toScenePoint);


public slots:
	//TODO: proxy these into pUi->MainWindow so they appear in Design.
	void cropCornerMoved(quint8 iCorner);
	void cropLineMoved();
	void hideCrop();
	void loadImage(const QString &sPathFile);
	void openPath(const QString &sPath);
	void showCrop();

}; // QtSssSiCmainWindow



}	} // namespace SwissalpS::QtSssSiC



#endif // QTSSSSICMAINWINDOW_H
