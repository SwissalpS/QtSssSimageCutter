#ifndef QTSSSSICCROPCORNER_H
#define QTSSSSICCROPCORNER_H

#include <QObject>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>



namespace SwissalpS { namespace QtSssSiC {



class QtSssSiCcropCorner : public QObject, public QGraphicsEllipseItem {

	Q_OBJECT

private:
	quint8 iCorner;
	bool bMoveInitiatedViaSetPosition;

protected:
	void initInstance();
	virtual QVariant itemChange(GraphicsItemChange oChange, const QVariant &oValue);

public:
	// main constructor
	explicit QtSssSiCcropCorner(quint8 iCorner = 0u, QGraphicsItem *pParent = 0);
	~QtSssSiCcropCorner();

signals:
	void wasMoved(quint8 iCorner);

public slots:
	void setPosition(qreal fX, qreal fY);

}; // QtSssSiCcropCorner



}	} // namespace SwissalpS::QtSssSiC



#endif // QTSSSSICCROPCORNER_H
