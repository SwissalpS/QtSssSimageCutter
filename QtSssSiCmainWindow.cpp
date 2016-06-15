#include "QtSssSiCmainWindow.h"
#include "ui_QtSssSiCmainWindow.h"
#include "QtSssSiCcropCorner.h"
#include "QtSssSiCcropLine.h"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <qgraphicsitem.h>
#include <QStandardPaths>
#include <QImageWriter>
#include <limits>



namespace SwissalpS { namespace QtSssSiC {



QtSssSiCmainWindow::QtSssSiCmainWindow(QWidget *parent) :
	QMainWindow(parent),
	pUI(new Ui::QtSssSiCmainWindow),
	pFSM(new QFileSystemModel),
	pGS(new QGraphicsScene),
	pCurrentImage(0),
	sPathFileCurrent(0),
	pRubberSelection(new QRectF),
	oPenCropBoxOutline(QPen(Qt::NoPen)),
	oBrushCropBoxFill(QBrush(Qt::gray, Qt::DiagCrossPattern)),
	pCCBottomLeft(new QtSssSiCcropCorner(3u)),
	pCCBottomRight(new QtSssSiCcropCorner(2u)),
	pCCTopLeft(new QtSssSiCcropCorner(0u)),
	pCCTopRight(new QtSssSiCcropCorner(1u)),
	pCLBottom(new QtSssSiCcropLine(true)),
	pCLLeft(new QtSssSiCcropLine(false)),
	pCLRight(new QtSssSiCcropLine(false)),
	pCLTop(new QtSssSiCcropLine(true)),
	pGRBottom(0),
	pGRLeft(0),
	pGRRight(0),
	pGRTop(0),
	pGPI(0),
	bShowingCrop(false),
	bImageChanged(false) {

	this->pUI->setupUi(this);

	this->initActions();

	this->initTreeView();

	this->initGraphicsView();

} // __constructor


QtSssSiCmainWindow::~QtSssSiCmainWindow() {

	delete this->pCurrentImage;
	delete this->pRubberSelection;
	delete this->pCLBottom;
	delete this->pCLLeft;
	delete this->pCLRight;
	delete this->pCLTop;
	delete this->pGRBottom;
	delete this->pGRLeft;
	delete this->pGRRight;
	delete this->pGRTop;
	delete this->pGPI;
	delete this->pGS;
	delete this->pFSM;
	delete this->pUI;

} // __destructor


void QtSssSiCmainWindow::initActions() {
	//qDebug() << "initActions";

	QAction *pAO = this->pUI->action_Open;
	pAO->setStatusTip(tr("Open a Folder"));
	connect(pAO, SIGNAL(triggered()), this, SLOT(onMenuOpen()));

	QAction *pAQ = this->pUI->action_Quit;
	pAQ->setStatusTip(tr("Quit Application"));
	connect(pAQ, SIGNAL(triggered()), this, SLOT(onMenuQuit()));

	// delete button is connected with .ui

} // initActions


void QtSssSiCmainWindow::initGraphicsView() {
	//qDebug() << "initGraphicsView";

	// connect view and scene
	this->pUI->graphicsView->setScene(this->pGS);

	// connect moved-signals of corners
	connect(this->pCCBottomLeft, SIGNAL(wasMoved(quint8)),
			this, SLOT(cropCornerMoved(quint8)));
	connect(this->pCCBottomRight, SIGNAL(wasMoved(quint8)),
			this, SLOT(cropCornerMoved(quint8)));
	connect(this->pCCTopLeft, SIGNAL(wasMoved(quint8)),
			this, SLOT(cropCornerMoved(quint8)));
	connect(this->pCCTopRight, SIGNAL(wasMoved(quint8)),
			this, SLOT(cropCornerMoved(quint8)));

	// connect moved-signals of lines
	connect(this->pCLBottom, SIGNAL(wasMoved()), this, SLOT(cropLineMoved()));
	connect(this->pCLLeft, SIGNAL(wasMoved()), this, SLOT(cropLineMoved()));
	connect(this->pCLRight, SIGNAL(wasMoved()), this, SLOT(cropLineMoved()));
	connect(this->pCLTop, SIGNAL(wasMoved()), this, SLOT(cropLineMoved()));

	// create crop box instances to scene
	this->pGRTop = this->pGS->addRect(0.0, 0.0, 1.0, 1.0,
									  this->oPenCropBoxOutline,
									  this->oBrushCropBoxFill);
	this->pGRBottom = this->pGS->addRect(0.0, 0.0, 1.0, 1.0,
										 this->oPenCropBoxOutline,
										 this->oBrushCropBoxFill);
	this->pGRLeft = this->pGS->addRect(0.0, 0.0, 1.0, 1.0,
									   this->oPenCropBoxOutline,
									   this->oBrushCropBoxFill);
	this->pGRRight = this->pGS->addRect(0.0, 0.0, 1.0, 1.0,
										this->oPenCropBoxOutline,
										this->oBrushCropBoxFill);

	// put the boxes just bellow the crop-lines and corners
	qreal iZ = std::numeric_limits<qreal>::max() -2;

	this->pGRBottom->setZValue(iZ);
	this->pGRLeft->setZValue(iZ);
	this->pGRRight->setZValue(iZ);
	this->pGRTop->setZValue(iZ);

	// Z-Value does not seem to have an effect,
	// farmore the order in which the graphic-items
	// are added to scene!

	// add crop-lines to scene
	this->pGS->addItem(this->pCLBottom);
	this->pGS->addItem(this->pCLLeft);
	this->pGS->addItem(this->pCLRight);
	this->pGS->addItem(this->pCLTop);

	// add crop-corners to scene
	this->pGS->addItem(this->pCCBottomLeft);
	this->pGS->addItem(this->pCCBottomRight);
	this->pGS->addItem(this->pCCTopLeft);
	this->pGS->addItem(this->pCCTopRight);

	this->hideCrop();

} // initGraphicsView


void QtSssSiCmainWindow::initTreeView() {
	//qDebug() << "initTreeView";

	// setup file-system-model

	this->oCurrentSelectedIndex = QModelIndex();

	// show files and folders that do not match name filter
	// so we can still change directories
	this->pFSM->setNameFilterDisables(true);

	// don't allow moving or deleting
	this->pFSM->setReadOnly(true);

	this->pFSM->setRootPath("");

	// these did not work so I set large width in ui-designer
	//this->pFSM->removeColumn(1); // size
	//this->pFSM->removeColumn(2); // type
	//this->pFSM->removeColumn(3); // date

	// filter what to show
	this->pFSM->setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);

	// filter out anything that isn't an image
	QStringList lFilters;
	QList<QByteArray> lBA = QImageWriter::supportedImageFormats();

	for(int i = 0; i < lBA.size(); ++i) {

		QString str(lBA[i].constData());
		lFilters << "*." + str;

	} // loop all valid formats
	//qDebug() << lFilters; //--> "*.bmp", "*.ico", "*.jpeg", "*.jpg", "*.pbm", "*.pgm", "*.png", "*.ppm", "*.tif", "*.tiff", "*.wbmp", "*.xbm", "*.xpm"

	// documentation says: can't save << "*.gif" << "*.pbm" << "*.pgm"
	// I'll trust the output of QImageWriter. This may be platform dependent?

	this->pFSM->setNameFilters(lFilters);

	// setup tree-view
	this->pUI->treeView->setModel(this->pFSM);
	this->pUI->treeView->setRootIsDecorated(true);

	// start with users pictures directory
	QStringList lPaths = QStandardPaths::standardLocations(
							 QStandardPaths::PicturesLocation);
	if (!lPaths.empty()) {

		this->openPath(lPaths.first());

	} // if found at least one valid path

	// connect signals and slots
	//connect(this->pUI->treeView, SIGNAL(clicked(QModelIndex)), this,
		//	SLOT(onClick(QModelIndex)));
	connect(this->pUI->treeView, SIGNAL(doubleClicked(QModelIndex)),
			this, SLOT(onDoubleClick(QModelIndex)));
	connect(this->pUI->treeView->selectionModel(),
			SIGNAL(currentChanged(QModelIndex, QModelIndex)),
			this, SLOT(onTreeSelectionChanged(QModelIndex, QModelIndex)));

} // initTreeView


QRectF QtSssSiCmainWindow::rubberRectNormalized() {
	//qDebug() << "rubberRectNormalized";

	QRectF oRectScene = this->pGS->sceneRect();
	qreal fBottom, fRight, fX1, fX2, fY1, fY2;
	fBottom = oRectScene.bottom();
	fRight = oRectScene.right();

	// read coordinates from selection
	fX1 = this->pRubberSelection->left();
	fX2 = this->pRubberSelection->right();
	fY1 = this->pRubberSelection->top();
	fY2 = this->pRubberSelection->bottom();

	// adjust overflows
	if (fX1 > fRight) fX1 = fRight;
	if (fX2 > fRight) fX2 = fRight;
	if (fY1 > fBottom) fY1 = fBottom;
	if (fY2 > fBottom) fY2 = fBottom;

	// adjust underflows
	if (0 > fX1) fX1 = 0;
	if (0 > fX2) fX2 = 0;
	if (0 > fY1) fY1 = 0;
	if (0 > fY2) fY2 = 0;

	return QRectF(QPointF(fX1, fY1), QPointF(fX2, fY2));

} // rubberRectNormalized


void QtSssSiCmainWindow::rotateImage(qreal fAngle) {
	//qDebug() << "rotateImage" << fAngle;

	if (!this->pCurrentImage) return;

	this->bImageChanged = true;

	this->hideCrop();

	QTransform oTransform;
	oTransform.rotate(fAngle);
	this->pCurrentImage = new QImage(this->pCurrentImage->transformed(oTransform));

	this->updatePixmap();

	this->updateGraphicsView();
	this->updateLandscapeIndicator();

} // rotateImage


void QtSssSiCmainWindow::rubberReleased() {
	//qDebug() << "rubber released";

	// position the crop corners
	this->updateCropCorners();

	// position the crop lines
	this->updateCropLines();

	// position the crop boxes
	this->updateCropBoxes();

	// show rubber
	this->showCrop();

	// show status (hor/ver/sq)
	this->updateStatusMessage();

} // rubberReleased


void QtSssSiCmainWindow::saveImage() {
	//qDebug() << "saveImage";

	if (!this->pCurrentImage) return;

	this->pCurrentImage->save(*this->sPathFileCurrent, 0,
							  this->pUI->sliderQuality->value());

} // saveImage


void QtSssSiCmainWindow::saveAndDestroyImage() {
	//qDebug() << "saveAndDestroyImage";

	if (this->bImageChanged) {

		this->saveImage();

	} // if any changes to save

	if (this->pGPI) {

		delete this->pGPI;
		this->pGPI = 0;

	} // if have already loaded an image

	if (this->pCurrentImage) {

		delete this->pCurrentImage;
		this->pCurrentImage = 0;

	} // if have an image loaded

	if (this->sPathFileCurrent) {

		delete this->sPathFileCurrent;
		this->sPathFileCurrent = 0;

	} // if have an image path

	this->bImageChanged = false;

} // saveAndDestroyImage


void QtSssSiCmainWindow::setSelection(QRectF &oNewRect) {
	//qDebug() << "setSelection";

	// destroy old selection
	delete this->pRubberSelection;
	this->pRubberSelection = 0;

	// normalize rect and set as new selection
	this->pRubberSelection = new QRectF(oNewRect.normalized());

} // setSelection


void QtSssSiCmainWindow::showCrop() {
	//qDebug() << "showCrop";

	this->bShowingCrop = true;

	// make boxes visible
	this->pGRBottom->show();
	this->pGRLeft->show();
	this->pGRRight->show();
	this->pGRTop->show();

	// make crop corners visible
	this->pCCBottomLeft->show();
	this->pCCBottomRight->show();
	this->pCCTopLeft->show();
	this->pCCTopRight->show();

	// make crop lines visible
	this->pCLBottom->show();
	this->pCLLeft->show();
	this->pCLRight->show();
	this->pCLTop->show();

} // showCrop


void QtSssSiCmainWindow::updateCropBoxes() {
	//qDebug() << "updateCropBoxes";

	QRectF oRectRubber = this->rubberRectNormalized();
	QRectF oRectScene = this->pGS->sceneRect();
	qreal fBottom, fRight, fX1, fX2, fY1, fY2;
	fBottom = oRectScene.bottom();
	fRight = oRectScene.right();

	// read coordinates from selection
	fX1 = oRectRubber.left();
	fX2 = oRectRubber.right();
	fY1 = oRectRubber.top();
	fY2 = oRectRubber.bottom();

	// adjust rectangles
	this->pGRBottom->setRect(0.0, fY2, fRight, fBottom - fY2);
	this->pGRLeft->setRect(0.0, fY1, fX1, fY2 - fY1);
	this->pGRRight->setRect(fX2, fY1, fRight - fX2, fY2 - fY1);
	this->pGRTop->setRect(0.0, 0.0, fRight, fY1);

} // updateCropBoxes


void QtSssSiCmainWindow::updateCropCorners() {
	//qDebug() << "updateCropCorners";

	QRectF oRectRubber = this->rubberRectNormalized();

	// position the crop corners
	this->pCCBottomLeft->setPosition(oRectRubber.left(), oRectRubber.bottom());
	this->pCCBottomRight->setPosition(oRectRubber.right(), oRectRubber.bottom());
	this->pCCTopLeft->setPosition(oRectRubber.left(), oRectRubber.top());
	this->pCCTopRight->setPosition(oRectRubber.right(), oRectRubber.top());

} // updateCropCorners


void QtSssSiCmainWindow::updateCropLines() {
	//qDebug() << "updateCropLines";

	QRectF oRectRubber = this->rubberRectNormalized();

	// position the crop lines
	this->pCLBottom->setPosition(oRectRubber.bottom());
	this->pCLLeft->setPosition(oRectRubber.left());
	this->pCLRight->setPosition(oRectRubber.right());
	this->pCLTop->setPosition(oRectRubber.top());

} // updateCropLines


void QtSssSiCmainWindow::updateGraphicsView() {
	//qDebug() << "updateGraphicsView";

	this->pUI->graphicsView->fitInView(
				this->pGS->sceneRect(), Qt::KeepAspectRatio);
	//			this->pGS->itemsBoundingRect(), Qt::KeepAspectRatio); // <-- slower

} // updateGraphicsView


void QtSssSiCmainWindow::updateLandscapeIndicator() {
	//qDebug() << "updateLandscapeIndicator";

	if (!this->pCurrentImage) return;

	int iHeight = this->pCurrentImage->height();
	int iWidth = this->pCurrentImage->width();
	QRadioButton *pRadio = this->pUI->radioButtonIsLandscape;

	if (iWidth < iHeight) {

		pRadio->setChecked(false);

	} else {

		pRadio->setChecked(true);

	} // if not landscape

	// adapt label in case we have a square
	if (iWidth == iHeight) {

		pRadio->setText(tr("is square"));

	} else {

		pRadio->setText(tr("is landscape"));

	} // if square or not

} // updateLandscapeIndicator


void QtSssSiCmainWindow::updatePixmap() {
	//qDebug() << "updatePixmap";

	if (!this->pCurrentImage) return;

	if (this->pGPI) {

		delete this->pGPI; this->pGPI = 0;

	} // if have already loaded an image

	this->pGPI = new QGraphicsPixmapItem(QPixmap::fromImage(*this->pCurrentImage));
	this->pGS->addItem(this->pGPI);
	this->pGS->setSceneRect(this->pCurrentImage->rect());

} // updatePixmap


void QtSssSiCmainWindow::updateStatusMessage(){
	//qDebug() << "updateStatusMessage";

	QRectF oRectRubber = this->rubberRectNormalized();

	// prepare status bar message
	QString sStatus;
	QString sLorP = "    | ";
	int iHeight = (int)oRectRubber.height();
	int iWidth = (int)oRectRubber.width();

	if (iHeight < iWidth) {

		sLorP = " -- ";
		sStatus = tr("crop ratio is landscape");

	} else if (iHeight == iWidth) {


		sLorP = " |=| ";
		sStatus = tr("crop aspect is square");

	} else {

		sStatus = tr("crop aspect is portrait");

	} // if crop has landscape, portrait or square ratio

	// show the message about ratio
	this->pUI->statusBar->showMessage(sStatus + " " +
									  QString::number(iWidth, 2)
									  + sLorP + QString::number(iHeight, 2)
									  );

} // updateStatusMessage


void QtSssSiCmainWindow::contextMenuEvent(QContextMenuEvent *event) {
	//qDebug() << "contextMenuEvent" << event;
	Q_UNUSED(event);

} // contextMenuEvent


void QtSssSiCmainWindow::onMenuOpen() {
	//qDebug() << "onMenuOpen";

	QString sPath = QFileDialog::getExistingDirectory(this,
													  tr("select Folder"),
													  QDir::currentPath());

	this->openPath(sPath);

} // onMenuOpen


void QtSssSiCmainWindow::onMenuQuit() {
	//qDebug() << "quit";

	QApplication::quit();

} // onMenuQuit


void QtSssSiCmainWindow::openPath(const QString &sPath) {
	//qDebug() << "openPath" << sPath;

	// no need to attempt if nothing given
	if (sPath.isEmpty()) return;

	// find index of given path
	QModelIndex oPathIndex = this->pFSM->index(sPath);

	// show new path as base
	this->pUI->treeView->setRootIndex(oPathIndex);

} // openPath


// this is no longer required, may need it again though, renamed...
void QtSssSiCmainWindow::onClick(QModelIndex index) {
	//qDebug() << "onClick" << index << this->pFSM->filePath(index);

	// we can ignore single-clicks on directories
	if (this->pFSM->isDir(index)) return;

	//qDebug() << "isFile";

	this->loadImage(this->pFSM->filePath(index));

} // onClick


void QtSssSiCmainWindow::onTreeSelectionChanged(const QModelIndex &current, const QModelIndex &previous) {
	//qDebug() << "onTreeSelectionChanged" << current << previous;

	// we can ignore if both current and previous is same row
	// this can be excluded with setting in ui-designer
	if (current.row() == previous.row()) return;

	// we can ignore directories
	if (this->pFSM->isDir(current)) return;

	//qDebug() << "isFile";
	//TODO: ? check if same? If there is only one image, this may be the only way to save it. Maybe it's best anyway to simply save at each action.

	// this may generate unknown behaviour if the new selection is not a valid image
	this->oCurrentSelectedIndex = current;

	// attempt to load the currently selected item
	this->loadImage(this->pFSM->filePath(current));

} // onTreeSelectionChanged


void QtSssSiCmainWindow::onDoubleClick(QModelIndex index) {
	//qDebug() << "onDoubleClick" << index << this->pFSM->filePath(index);

	// we only treat directories for double click
	if (!this->pFSM->isDir(index)) return;

	//qDebug() << "isDir";

	this->openPath(this->pFSM->filePath(index));

} // onDoubleClick


void QtSssSiCmainWindow::on_buttonPrevious_clicked() {
	//qDebug() << "prev";

	QModelIndex previousIndex = this->pUI->treeView->indexAbove(
									this->oCurrentSelectedIndex);

	//TODO: check both prev and next on change to disable buttons
	if (!previousIndex.isValid()) return;

	this->pUI->treeView->setCurrentIndex(previousIndex);

} // on_buttonPrevious_clicked


void QtSssSiCmainWindow::on_buttonRotateCCW_clicked() {
	//qDebug() << "CCW";

	this->rotateImage(-90);

} // on_buttonRotateCCW_clicked


void QtSssSiCmainWindow::on_buttonCrop_clicked() {
	//qDebug() << "crop";

	if (!this->pCurrentImage) return;
	if (!this->bShowingCrop) return;

	this->bImageChanged = true;

	// fetch normalized rubber rect
	QRect oRectRubber = this->rubberRectNormalized().toRect();

	this->hideCrop();

	// crop image
	QImage oImageCropped = this->pCurrentImage->copy(oRectRubber);
	this->pCurrentImage = new QImage(oImageCropped);

	this->updatePixmap();

	this->updateGraphicsView();
	this->updateLandscapeIndicator();

} // on_buttonCrop_clicked


void QtSssSiCmainWindow::on_buttonRotateCW_clicked() {
	//qDebug() << "rCW";

	this->rotateImage(90);

} // on_buttonRotateCW_clicked


void QtSssSiCmainWindow::on_buttonNext_clicked() {
	//qDebug() << "next";

	QModelIndex nextIndex = this->pUI->treeView->indexBelow(
								this->oCurrentSelectedIndex);

	//TODO: check both prev and next on change to disable buttons
	if (!nextIndex.isValid()) return;

	this->pUI->treeView->setCurrentIndex(nextIndex);

} // on_buttonNext_clicked


void QtSssSiCmainWindow::on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint) {
	//qDebug() << "rubber" << viewportRect << fromScenePoint << toScenePoint;

	if (!this->pCurrentImage) return;

	if (viewportRect.isNull()) {

		this->rubberReleased();

		return;

	} // if null -> aka released

	QRectF oRect(fromScenePoint, toScenePoint);
	this->setSelection(oRect);

} // on_graphicsView_rubberBandChanged


void QtSssSiCmainWindow::loadImage(const QString &sPathFile) {
	//qDebug() << "loadImage" << sPathFile;

	this->hideCrop();

	QImage oImage(sPathFile);

	if (oImage.isNull()) {

		qDebug() << "not valid image" << sPathFile;
		return;

	} // if invalid image file

	//this->pUI->graphicsView->setWindowFilePath(sPathFile);

	// save, and destroy pointers
	this->saveAndDestroyImage();

	this->sPathFileCurrent = new QString(sPathFile);
	this->pCurrentImage = new QImage(sPathFile);

	this->updatePixmap();

	this->updateGraphicsView();
	this->updateLandscapeIndicator();

} // loadImage


void QtSssSiCmainWindow::hideCrop() {
	//qDebug() << "hideCrop";

	this->bShowingCrop = false;

	// hide crop corners
	this->pCCBottomLeft->hide();
	this->pCCBottomRight->hide();
	this->pCCTopLeft->hide();
	this->pCCTopRight->hide();

	// hide crop lines
	this->pCLBottom->hide();
	this->pCLLeft->hide();
	this->pCLRight->hide();
	this->pCLTop->hide();

	// hide crop boxes
	this->pGRBottom->hide();
	this->pGRLeft->hide();
	this->pGRRight->hide();
	this->pGRTop->hide();

} // hideCrop


void QtSssSiCmainWindow::resizeEvent(QResizeEvent *event) {
	//qDebug() << "resizeEvent";

	QMainWindow::resizeEvent(event);

	this->updateGraphicsView();

} // resizeEvent


void QtSssSiCmainWindow::on_checkBoxExpandAll_stateChanged(int iState) {
	//qDebug() << "on_checkBoxExpandAll_stateChanged" << iState;

	if (0 == iState) {

		// not checked -> collapse
		this->pUI->treeView->collapseAll();

	} else {

		// checked or mixed -> expand all
		this->pUI->treeView->expandAll();

	} // switch sate

} // on_checkBoxExpandAll_stateChanged


void QtSssSiCmainWindow::on_actionDelete_triggered() {
	//qDebug() << "on_actionDelete_triggered";

	if (!this->pCurrentImage) return;

	this->hideCrop();

	// delete forever, no trash
	QFile::remove(*this->sPathFileCurrent);

	// avoid saving after having just been deleted
	this->bImageChanged = false;

	// remove pointers etc.
	this->saveAndDestroyImage();

} // on_actionDelete_triggered


void QtSssSiCmainWindow::cropCornerMoved(quint8 iCorner) {
	//qDebug() << "cropCornerMoved" << iCorner;

	if (!this->pCurrentImage) return;

	qreal fX1, fX2, fY1, fY2;
	fX1 = this->pCCTopLeft->scenePos().x();
	fY1 = this->pCCTopLeft->scenePos().y();
	fX2 = this->pCCBottomRight->scenePos().x();
	fY2 = this->pCCBottomRight->scenePos().y();

	switch (iCorner) {

		case 0u:
			// top-left-corner
		case 2u:
			// bottom-right-corner
			break;

		case 1u:
			// top-right-corner
			fY1 = this->pCCTopRight->scenePos().y();
			fX2 = this->pCCTopRight->scenePos().x();
			break;


		case 3u:
			// bottom-left-corner
			fX1 = this->pCCBottomLeft->scenePos().x();
			fY2 = this->pCCBottomLeft->scenePos().y();
			break;

		default:
			return;
			break;

	} // switch iCorner

	QRectF oRect(QPointF(fX1, fY1), QPointF(fX2, fY2));
	this->setSelection(oRect);

	// update the other markers
	this->updateCropBoxes();

	this->updateCropCorners();

	this->updateCropLines();

	this->updateStatusMessage();

} // cropCornerMoved


void QtSssSiCmainWindow::cropLineMoved() {
	//qDebug() << "cropLineMoved";

	QRectF oRect(QPointF(this->pCLLeft->scenePos().x(),
						 this->pCLTop->scenePos().y()),
				 QPointF(this->pCLRight->scenePos().x(),
						 this->pCLBottom->scenePos().y()));
	this->setSelection(oRect);

	this->updateCropBoxes();

	this->updateCropCorners();

	//this->updateCropLines();

	this->updateStatusMessage();

} // cropLineMoved



}	} // namespace SwissalpS::QtSssSiC
