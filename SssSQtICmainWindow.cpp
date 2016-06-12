#include "SssSQtICmainWindow.h"
#include "ui_SssSQtICmainWindow.h"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <qgraphicsitem.h>
#include <QStandardPaths>


SssSQtICmainWindow::SssSQtICmainWindow(QWidget *parent) :
	QMainWindow(parent),
	pUI(new Ui::SssSQtICmainWindow),
	pFSM(new QFileSystemModel),
	pGS(new QGraphicsScene),
	pCurrentImage(0),
	sPathFileCurrent(0),
	pRubberSelection(new QRectF),
	oPenCropLines(QPen(QBrush(Qt::red, Qt::SolidPattern), 3.0)),
	pGLBottom(0),
	pGLLeft(0),
	pGLRight(0),
	pGLTop(0),
	pGPI(0),
	bShowingCrop(false),
	bImageChanged(false) {

	this->pUI->setupUi(this);

	this->initActions();

	this->initTreeView();

	this->initGraphicsView();

} // __constructor


SssSQtICmainWindow::~SssSQtICmainWindow() {

	delete pUI;

} // __destructor


void SssSQtICmainWindow::initActions() {
	qDebug() << "initActions";

	QAction *pAO = this->pUI->action_Open;
	pAO->setStatusTip(tr("Open a Folder"));
	connect(pAO, SIGNAL(triggered()), this, SLOT(onMenuOpen()));

	QAction *pAQ = this->pUI->action_Quit;
	pAQ->setStatusTip(tr("Quit Application"));
	connect(pAQ, SIGNAL(triggered()), this, SLOT(onMenuQuit()));

} // initActions


void SssSQtICmainWindow::initGraphicsView() {
	//qDebug() << "initGraphicsView";

	this->pUI->graphicsView->setScene(this->pGS);

} // initGraphicsView


void SssSQtICmainWindow::initTreeView() {
	qDebug() << "initTreeView";

	// setup file-system-model

	this->oCurrentSelectedIndex = QModelIndex();

	// show files and folders that do not match name filter
	// so we can still change directories
	this->pFSM->setNameFilterDisables(true);

	// don't allow moving or deleting
	this->pFSM->setReadOnly(true);

	this->pFSM->setRootPath("/home /root /mnt");

	// these did not work so I set large width in designer
	this->pFSM->removeColumn(1); // size
	this->pFSM->removeColumn(2); // type
	this->pFSM->removeColumn(3); // date

	// filter what to show
	this->pFSM->setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);

	// filter out anything that isn't an image
	QStringList lFilters;
	lFilters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif"
			 << "*.pbm" << "*.pgm" << "*.ppm" << "*.xbm" << "*.xpm" << "*.svg";
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


QRectF SssSQtICmainWindow::rubberRectNormalized() {
	//qDebug() << "rubberRectNormalized";

	QRectF oRectScene = this->pGS->sceneRect();
	qreal fBottom, fRight, fX1, fX2, fY1, fY2;
	fBottom = oRectScene.bottom();
	fRight = oRectScene.right();

	// read coordinates from rubber
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


void SssSQtICmainWindow::rotateImage(qreal fAngle) {
	//qDebug() << "rotateImage" << fAngle;

	if (!this->pCurrentImage) return;

	this->bImageChanged = true;

	this->removeCropMarker();

	QTransform oTransform;
	oTransform.rotate(fAngle);
	this->pCurrentImage = new QImage(this->pCurrentImage->transformed(oTransform));

	this->updatePixmap();

	this->updateGraphicsView();
	this->updateLandscapeIndicator();

} // rotateImage


void SssSQtICmainWindow::rubberReleased() {
	//qDebug() << "rubber released";

	this->removeCropMarker();

	// show rubber
	this->bShowingCrop = true;

	QRectF oRectRubber = this->rubberRectNormalized();
	QRectF oRectScene = this->pGS->sceneRect();
	qreal fBottom, fRight, fX1, fX2, fY1, fY2;
	fBottom = oRectScene.bottom();
	fRight = oRectScene.right();

	// read coordinates from rubber
	fX1 = oRectRubber.left();
	fX2 = oRectRubber.right();
	fY1 = oRectRubber.top();
	fY2 = oRectRubber.bottom();

	// add horizontal lines
	this->pGLTop = this->pGS->addLine(0, fY1, fRight, fY1,
									  this->oPenCropLines);
	this->pGLBottom = this->pGS->addLine(0, fY2, fRight, fY2,
										 this->oPenCropLines);

	// vertical lines
	this->pGLLeft = this->pGS->addLine(fX1, 0, fX1, fBottom,
									   this->oPenCropLines);
	this->pGLRight = this->pGS->addLine(fX2, 0, fX2, fBottom,
										this->oPenCropLines);

} // rubberReleased


void SssSQtICmainWindow::saveImage() {
	//qDebug() << "saveImage";

	if (!this->pCurrentImage) return;

	this->pCurrentImage->save(*this->sPathFileCurrent);

} // saveImage


void SssSQtICmainWindow::saveAndDestroyImage() {
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
	this->iRotation = 0;

} // saveAndDestroyImage


void SssSQtICmainWindow::updateGraphicsView() {
	//qDebug() << "updateGraphicsView";

	this->pUI->graphicsView->fitInView(
				this->pGS->sceneRect(), Qt::KeepAspectRatio);

	//this->pUI->graphicsView->fitInView(
	//			this->pGS->itemsBoundingRect(), Qt::KeepAspectRatio);

} // updateGraphicsView


void SssSQtICmainWindow::updateLandscapeIndicator() {
	//qDebug() << "updateLandscapeIndicator";

	if (!this->pCurrentImage) return;

	this->pUI->radioButtonIsLandscape->setChecked(
				this->pCurrentImage->width() > this->pCurrentImage->height());

} // updateLandscapeIndicator


void SssSQtICmainWindow::updatePixmap() {
	//qDebug() << "updatePixmap";

	if (!this->pCurrentImage) return;

	if (this->pGPI) {

		delete this->pGPI; this->pGPI = 0;

	} // if have already loaded an image

	this->pGPI = new QGraphicsPixmapItem(QPixmap::fromImage(*this->pCurrentImage));
	this->pGS->addItem(this->pGPI);
	this->pGS->setSceneRect(this->pCurrentImage->rect());

} // updatePixmap


void SssSQtICmainWindow::contextMenuEvent(QContextMenuEvent *event) {
	qDebug() << "contextMenuEvent" << event;

} // contextMenuEvent


void SssSQtICmainWindow::onMenuOpen() {
	//qDebug() << "onMenuOpen";

	QString sPath = QFileDialog::getExistingDirectory(this,
													  tr("select Folder"),
													  QDir::currentPath());

	this->openPath(sPath);

} // onMenuOpen


void SssSQtICmainWindow::onMenuQuit() {
	qDebug() << "quit";

	QApplication::quit();

} // onMenuQuit


void SssSQtICmainWindow::openPath(const QString &sPath) {
	//qDebug() << "openPath" << sPath;

	// no need to attempt if nothing given
	if (sPath.isEmpty()) return;

	// find index of given path
	QModelIndex oPathIndex = this->pFSM->index(sPath);

	// show new path as base
	this->pUI->treeView->setRootIndex(oPathIndex);

} // openPath


// these two can probably go too as I now have something working
void SssSQtICmainWindow::opened(const QString &newPath) {
	qDebug() << "opened" << newPath;

} // opened


void SssSQtICmainWindow::opened2(const QString &path) {
	qDebug() << "opened2" << path;

} // opened2


// this is no longer required
void SssSQtICmainWindow::onClick(QModelIndex index) {
	//qDebug() << "onClick" << index << this->pFSM->filePath(index);

	// we can ignore single-clicks on directories
	if (this->pFSM->isDir(index)) return;

	//qDebug() << "isFile";

	this->loadImage(this->pFSM->filePath(index));

} // onClick


void SssSQtICmainWindow::onTreeSelectionChanged(const QModelIndex &current, const QModelIndex &previous) {
	//qDebug() << "onTreeSelectionChanged" << current << previous;

	// we can ignore if both current and previous is same row
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


void SssSQtICmainWindow::onDoubleClick(QModelIndex index) {
	//qDebug() << "onDoubleClick" << index << this->pFSM->filePath(index);

	// we only treat directories for double click
	if (!this->pFSM->isDir(index)) return;

	//qDebug() << "isDir";

	this->openPath(this->pFSM->filePath(index));

} // onDoubleClick


void SssSQtICmainWindow::on_buttonPrevious_clicked() {
	//qDebug() << "prev";

	QModelIndex previousIndex = this->pUI->treeView->indexAbove(
									this->oCurrentSelectedIndex);

	//TODO: check both prev and next on change to disable buttons
	if (!previousIndex.isValid()) return;

	this->pUI->treeView->setCurrentIndex(previousIndex);

} // on_buttonPrevious_clicked


void SssSQtICmainWindow::on_buttonRotateCCW_clicked() {
	//qDebug() << "CCW";

	this->rotateImage(-90);

} // on_buttonRotateCCW_clicked


void SssSQtICmainWindow::on_buttonCrop_clicked() {
	//qDebug() << "crop";

	if (!this->pCurrentImage) return;
	if (!this->bShowingCrop) return;

	this->bImageChanged = true;

	// fetch normalized rubber rect
	QRect oRectRubber = this->rubberRectNormalized().toRect();

	this->removeCropMarker();

	// crop image
	QImage oImageCropped = this->pCurrentImage->copy(oRectRubber);
	this->pCurrentImage = new QImage(oImageCropped);

	this->updatePixmap();

	this->updateGraphicsView();
	this->updateLandscapeIndicator();

} // on_buttonCrop_clicked


void SssSQtICmainWindow::on_buttonRotateCW_clicked() {
	//qDebug() << "rCW";

	this->rotateImage(90);

} // on_buttonRotateCW_clicked


void SssSQtICmainWindow::on_buttonNext_clicked() {
	//qDebug() << "next";

	QModelIndex nextIndex = this->pUI->treeView->indexBelow(
								this->oCurrentSelectedIndex);

	//TODO: check both prev and next on change to disable buttons
	if (!nextIndex.isValid()) return;

	this->pUI->treeView->setCurrentIndex(nextIndex);

} // on_buttonNext_clicked


void SssSQtICmainWindow::on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint) {
	//qDebug() << "rubber" << viewportRect << fromScenePoint << toScenePoint;

	if (viewportRect.isNull()) {

		this->rubberReleased();

		return;

	} // if null -> aka released

	// destroy old selection
	delete this->pRubberSelection; this->pRubberSelection = 0;

	// determine where the selection started
	if (fromScenePoint.x() < toScenePoint.x()) {

		this->pRubberSelection = new QRectF(fromScenePoint, toScenePoint);

	} else {

		this->pRubberSelection = new QRectF(toScenePoint, fromScenePoint);

	} // if started top-left or bottom-right

	//qDebug() << "selection" << this->pRubberSelection << fromScenePoint << toScenePoint;

} // on_graphicsView_rubberBandChanged


void SssSQtICmainWindow::loadImage(const QString &sPathFile) {
	//qDebug() << "loadImage" << sPathFile;

	this->removeCropMarker();

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


void SssSQtICmainWindow::removeCropMarker() {
	//qDebug() << "removeCropMarker";

	this->bShowingCrop = false;

	if (this->pGLBottom) {

		//qDebug() << "got bottom";
		delete this->pGLBottom; this->pGLBottom = 0;
		delete this->pGLLeft; this->pGLLeft = 0;
		delete this->pGLRight; this->pGLRight = 0;
		delete this->pGLTop; this->pGLTop = 0;

	} // if got crop lines

} // removeCropMarker


void SssSQtICmainWindow::resizeEvent(QResizeEvent *event) {
	//qDebug() << "resizeEvent";

	//QWidget::resizeEvent(event);
	QMainWindow::resizeEvent(event);

	this->updateGraphicsView();

} // resizeEvent


void SssSQtICmainWindow::on_checkBoxExpandAll_stateChanged(int iState) {
	//qDebug() << "on_checkBoxExpandAll_stateChanged" << iState;

	if (0 == iState) {

		// not checked -> collapse
		this->pUI->treeView->collapseAll();

	} else {

		// checked or mixed -> expand all
		this->pUI->treeView->expandAll();

	} // switch sate

} // on_checkBoxExpandAll_stateChanged
