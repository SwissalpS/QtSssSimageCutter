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
	pGPI(0) {

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
	qDebug() << "initGraphicsView";

	this->pGS->setSceneRect(this->pUI->graphicsView->rect());
	this->pUI->graphicsView->setScene(this->pGS);
	this->pUI->graphicsView->setResizeAnchor(QGraphicsView::NoAnchor);

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
	this->pFSM->setFilter(QDir::NoDotAndDotDot | QDir::AllEntries); // only dirs -> | QDir::AllDirs);

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


void SssSQtICmainWindow::contextMenuEvent(QContextMenuEvent *event) {
	qDebug() << "contextMenuEvent" << event;

} // contextMenuEvent


void SssSQtICmainWindow::onMenuOpen() {
	qDebug() << "open";

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
	qDebug() << "openPath" << sPath;

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
	qDebug() << "onClick" << index << this->pFSM->filePath(index);

	if (this->pFSM->isDir(index)) {

		qDebug() << "isDir";

	} else {

		qDebug() << "isFile";

		QImage oImage(this->pFSM->filePath(index));

		if (oImage.isNull()) {

			qDebug() << "not valid image";
			return;

		} // if invalid image file

		//this->pUI->graphicsView->setWindowFilePath(this->pFSM->filePath(index));

		if (this->pGPI) {

			delete this->pGPI;
			this->pGPI = 0;

		} // if have already loaded an image

		this->pGPI = new QGraphicsPixmapItem(QPixmap::fromImage(oImage));
		this->pGS->addItem(this->pGPI);
		//this->pUI->graphicsView->centerOn(this->pGPI);
		//this->pUI->graphicsView->fitInView(this->pGPI);
		this->pUI->graphicsView->fitInView(this->pGS->sceneRect(), Qt::KeepAspectRatio);

	} // if dir or file clicked

} // onClick


void SssSQtICmainWindow::onDoubleClick(QModelIndex index) {
	qDebug() << "onDoubleClick" << index << this->pFSM->filePath(index);

	// we only treat directories for double click
	if (!this->pFSM->isDir(index)) return;

	qDebug() << "isDir";

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

} // on_buttonNext_clicked


void SssSQtICmainWindow::on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint) {
	qDebug() << "rubber" << viewportRect << fromScenePoint << toScenePoint;

} // on_graphicsView_rubberBandChanged


void SssSQtICmainWindow::resizeEvent(QResizeEvent *event) {
	qDebug() << "resizeEvent";

	//QWidget::resizeEvent(event);
	QMainWindow::resizeEvent(event);

	this->pGS->setSceneRect(this->pUI->graphicsView->rect());
	this->pUI->graphicsView->fitInView(this->pGS->sceneRect(), Qt::KeepAspectRatio);

	if (!this->pGPI) return;

	//this->pUI->graphicsView->centerOn(this->pGPI);
	//this->pUI->graphicsView->fitInView(this->pGPI);

} // resizeEvent


void SssSQtICmainWindow::showEvent(QShowEvent *event) {
	qDebug() << "showEvent";

	this->pUI->graphicsView->fitInView(this->pGS->sceneRect(), Qt::KeepAspectRatio); //:KeepAspectRatioByExpanding);

} // showEvent
