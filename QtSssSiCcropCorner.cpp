#include "QtSssSiCcropCorner.h"

#include <QBrush>
#include <QCursor>
#include <QDebug>
#include <QGraphicsScene>
#include <QPen>
#include <limits>


namespace SwissalpS { namespace QtSssSiC {



void QtSssSiCcropCorner::initInstance() {

	this->bMoveInitiatedViaSetPosition = false;

	this->setPen(QPen(QBrush(Qt::green, Qt::SolidPattern), 3.0));
	this->setFlags(QGraphicsItem::ItemIsMovable
				   | QGraphicsItem::ItemSendsScenePositionChanges);

	// set cursor
	switch (this->iCorner) {

		case 0:
			// top-left
		case 2:
			// bottom-right
			this->setCursor(QCursor(Qt::SizeFDiagCursor));
			break;

		case 1:
			// top-right
		case 3:
			// bottom-left
			this->setCursor(QCursor(Qt::SizeBDiagCursor));
			break;

		default:
			//TODO: throw exception
			this->setCursor(QCursor(Qt::ForbiddenCursor));
			break;

	} // switch this->iCorner

	this->setRect(-2.5, -2.5, 5.0, 5.0);

	// make sure this item is on top
	this->setZValue(std::numeric_limits<qreal>::max());

} // initInstance


QVariant QtSssSiCcropCorner::itemChange(QGraphicsItem::GraphicsItemChange oChange, const QVariant &oValue) {
	//qDebug() << "itemChange" << oChange << oValue;

	if (ItemPositionChange == oChange && this->scene()) {
		//qDebug() << "itemChange" << oChange << oValue;

		// oValue is the new position.
		QPointF oNewPos = oValue.toPointF();

		// make sure we are not moved outside the scene
		QRectF oRect = this->scene()->sceneRect();
		if (!oRect.contains(oNewPos)) {

			oNewPos.setX(qMin(oRect.right(), qMax(oNewPos.x(), oRect.left())));
			oNewPos.setY(qMin(oRect.bottom(), qMax(oNewPos.y(), oRect.top())));

		} // if scene-rect does not contain new-position

		if (!this->bMoveInitiatedViaSetPosition) {

			// fun fact, these three lines do exactly the same (in this case)
			//this->wasMoved(this->iCorner);
			emit this->wasMoved(this->iCorner);
			//Q_EMIT this->wasMoved(this->iCorner);

		} // if got to emit signal
		this->bMoveInitiatedViaSetPosition = false;

		// return adjusted pos
		return oNewPos;

	} // if a move is about to happen

	// fall-back
	return QGraphicsItem::itemChange(oChange, oValue);

} // itemChange


// main constructor
QtSssSiCcropCorner::QtSssSiCcropCorner(quint8 iCorner, QGraphicsItem *pParent) :
	QGraphicsEllipseItem(pParent),
	iCorner(iCorner) {

	this->initInstance();

} // construct(quint8, QGraphicsItem)


QtSssSiCcropCorner::~QtSssSiCcropCorner() {


} // destructor


void QtSssSiCcropCorner::setPosition(qreal fX, qreal fY) {
	//qDebug() << "setPosition" << fX << fY;

	// avoid crash when we are not yet part of a scene
	if (!this->scene()) return;

	// set flag to avoid recursion when signal is emited
	this->bMoveInitiatedViaSetPosition = true;

	this->setPos(fX, fY);

} // setPosition



}	} // namespace SwissalpS::QtSssSiC
