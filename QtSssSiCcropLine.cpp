#include "QtSssSiCcropLine.h"

#include <QBrush>
#include <QCursor>
#include <QDebug>
#include <QGraphicsScene>
#include <QPen>
#include <limits>


namespace SwissalpS { namespace QtSssSiC {



void QtSssSiCcropLine::initInstance() {

	this->bMoveInitiatedViaSetPosition = false;

	this->setPen(QPen(QBrush(Qt::red, Qt::SolidPattern), 3.0));
	this->setFlags(QGraphicsItem::ItemIsMovable
				   | QGraphicsItem::ItemSendsScenePositionChanges);

	// set cursor and line
	if (this->bIsHorizontal) {

		this->setCursor(QCursor(Qt::SizeVerCursor));
		this->setLine(0.0, 0.0, 1.0, 0.0);

	} else {

		this->setCursor(QCursor(Qt::SizeHorCursor));
		this->setLine(0.0, 0.0, 0.0, 1.0);

	} // if horizontal or vertical

	// make sure this item is on top but under corners
	this->setZValue(std::numeric_limits<qreal>::max() -1);

	// make sure we are invisible at first
	//this->hide();

} // initInstance


QVariant QtSssSiCcropLine::itemChange(QGraphicsItem::GraphicsItemChange oChange, const QVariant &oValue) {
	//qDebug() << "itemChange" << oChange << oValue;

	if (ItemPositionChange == oChange && this->scene()) {
		//qDebug() << "itemChange" << oChange << oValue;

		// oValue is the new position.
		QPointF oNewPos = oValue.toPointF();
		QRectF oRect = this->scene()->sceneRect();

		// make sure we are not moved in the wrong direction
		// and not moving outside scene boundrary
		if (this->bIsHorizontal) {

			oNewPos.setX(oRect.left());
			oNewPos.setY(qMin(oRect.bottom(), qMax(oNewPos.y(), oRect.top())));

		} else {

			oNewPos.setX(qMin(oRect.right(), qMax(oNewPos.x(), oRect.left())));
			oNewPos.setY(oRect.top());

		} // if horizontal or vertical

		if (!this->bMoveInitiatedViaSetPosition) {

			// fun fact, these three lines do exactly the same (in this case)
			//this->wasMoved();
			emit this->wasMoved();
			//Q_EMIT this->wasMoved();

		} // if got to emit signal

		// clear flag
		this->bMoveInitiatedViaSetPosition = false;

		// return adjusted pos
		return oNewPos;

	} // if a move is about to happen

	// fall-back
	return QGraphicsItem::itemChange(oChange, oValue);

} // itemChange


// main constructor
QtSssSiCcropLine::QtSssSiCcropLine(bool bHorizontal, QGraphicsItem *pParent) :
	QGraphicsLineItem(pParent),
	bIsHorizontal(bHorizontal) {

	this->initInstance();

} // construct(bool, QGraphicsItem)


QtSssSiCcropLine::QtSssSiCcropLine(qreal fX1, qreal fY1, qreal fX2, qreal fY2, bool bHorizontal, QGraphicsItem *pParent) :
	QGraphicsLineItem(fX1, fY1, fX2, fY2, pParent),
	bIsHorizontal(bHorizontal) {

	this->initInstance();

} // construct(qreal, qreal, qreal, qreal, bool, QGraphicsItem *)


QtSssSiCcropLine::QtSssSiCcropLine(QGraphicsItem *pParent) :
	QGraphicsLineItem(pParent),
	bIsHorizontal(true) {

	this->initInstance();

} // construct(QGraphicsItem *)


QtSssSiCcropLine::QtSssSiCcropLine(const QLineF &oLine, QGraphicsItem *pParent) :
	QGraphicsLineItem(oLine, pParent) {

	//TODO: detect if line is horizontal or vertical or diagonal -> error

	this->initInstance();

} // construct(QLineF, QGraphicsItem *)


QtSssSiCcropLine::QtSssSiCcropLine(qreal fX1, qreal fY1, qreal fX2, qreal fY2, QGraphicsItem *pParent) :
	QGraphicsLineItem(fX1, fY1, fX2, fY2, pParent) {

	//TODO: detect if line is horizontal or vertical or diagonal -> error

	this->initInstance();

} // construct(qreal, qreal, qreal, qreal, QGraphicsItem *)


QtSssSiCcropLine::~QtSssSiCcropLine() {


} // destructor


void QtSssSiCcropLine::setPosition(qreal fXorY) {
	//qDebug() << "setPosition" << fXorY;

	// avoid crash when we are not yet part of a scene
	if (!this->scene()) return;

	// set flag to avoid recursion when signal is emited
	this->bMoveInitiatedViaSetPosition = true;

	QRectF oRectScene = this->scene()->sceneRect();
	QLineF oLine = this->line();

	// set line length and x or y pos
	if (this->bIsHorizontal) {

		oLine.setLength(oRectScene.width());
		this->setY(fXorY);

	} else {

		oLine.setLength(oRectScene.height());
		this->setX(fXorY);

	} // if horizontal or vertical

	// update line
	this->setLine(oLine);

} // setPosition



}	} // namespace SwissalpS::QtSssSiC
