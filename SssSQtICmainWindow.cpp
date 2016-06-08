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
	pGPI(0),
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

	//this->pGS->setSceneRect(this->pUI->graphicsView->rect());
	this->pUI->graphicsView->setScene(this->pGS);
	//this->pUI->graphicsView->setResizeAnchor(QGraphicsView::NoAnchor);

} // initGraphicsView


void SssSQtICmainWindow::initTreeView() {
	qDebug() << "initTreeView";

	// setup file-system-model

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
	connect(this->pUI->treeView, SIGNAL(clicked(QModelIndex)), this,
			SLOT(onClick(QModelIndex)));
	connect(this->pUI->treeView, SIGNAL(doubleClicked(QModelIndex)),
			this, SLOT(onDoubleClick(QModelIndex)));

} // initTreeView


void SssSQtICmainWindow::saveImage() {
	qDebug() << "TODO: saveImage";

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

} // saveAndDestroyImage


void SssSQtICmainWindow::updateGraphicsView() {
	//qDebug() << "updateGraphicsView";

	this->pUI->graphicsView->fitInView(
				this->pGS->sceneRect(), Qt::KeepAspectRatio);

} // updateGraphicsView


void SssSQtICmainWindow::updateLandscapeIndicator() {
	//qDebug() << "updateLandscapeIndicator";

	if (!this->pCurrentImage) return;

	this->pUI->radioButtonIsLandscape->setChecked(
				this->pCurrentImage->width() > this->pCurrentImage->height());

} // updateLandscapeIndicator


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


void SssSQtICmainWindow::opened(const QString &newPath) {
	qDebug() << "opened" << newPath;

} // opened


void SssSQtICmainWindow::opened2(const QString &path) {
	qDebug() << "opened2" << path;

} // opened2


void SssSQtICmainWindow::onClick(QModelIndex index) {
	//qDebug() << "onClick" << index << this->pFSM->filePath(index);

	// we can ignore single-clicks on directories
	if (this->pFSM->isDir(index)) return;

	//qDebug() << "isFile";

	this->loadImage(this->pFSM->filePath(index));

} // onClick


void SssSQtICmainWindow::onDoubleClick(QModelIndex index) {
	//qDebug() << "onDoubleClick" << index << this->pFSM->filePath(index);

	// we only treat directories for double click
	if (!this->pFSM->isDir(index)) return;

	//qDebug() << "isDir";

	this->openPath(this->pFSM->filePath(index));

} // onDoubleClick


void SssSQtICmainWindow::on_buttonPrevious_clicked() {
	qDebug() << "prev";

} // on_buttonPrevious_clicked


void SssSQtICmainWindow::on_buttonRotateCCW_clicked() {
	qDebug() << "CCW";

} // on_buttonRotateCCW_clicked


void SssSQtICmainWindow::on_buttonCrop_clicked() {
	qDebug() << "crop";

} // on_buttonCrop_clicked


void SssSQtICmainWindow::on_buttonRotateCW_clicked() {
	qDebug() << "rCW";

} // on_buttonRotateCW_clicked


void SssSQtICmainWindow::on_buttonNext_clicked() {
	qDebug() << "next";

	//this->pUI->treeView->

} // on_buttonNext_clicked


void SssSQtICmainWindow::on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint) {
	qDebug() << "rubber" << viewportRect << fromScenePoint << toScenePoint;

} // on_graphicsView_rubberBandChanged


void SssSQtICmainWindow::loadImage(const QString &sPathFile) {
	//qDebug() << "loadImage" << sPathFile;

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
	this->pGPI = new QGraphicsPixmapItem(QPixmap::fromImage(*this->pCurrentImage));
	this->pGS->addItem(this->pGPI);
	//this->pUI->graphicsView->centerOn(this->pGPI);
	//this->pUI->graphicsView->fitInView(this->pGPI);
	this->pGS->setSceneRect(this->pCurrentImage->rect());

	this->updateGraphicsView();
	this->updateLandscapeIndicator();

} // loadImage


void SssSQtICmainWindow::resizeEvent(QResizeEvent *event) {
	//qDebug() << "resizeEvent";

	//QWidget::resizeEvent(event);
	QMainWindow::resizeEvent(event);

	this->updateGraphicsView();

} // resizeEvent
