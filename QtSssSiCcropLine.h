#ifndef QTSSSSICCROPLINE_H
#define QTSSSSICCROPLINE_H

#include <QObject>
#include <QGraphicsLineItem>



namespace SwissalpS { namespace QtSssSiC {



class QtSssSiCcropLine : public QObject, public QGraphicsLineItem {

	Q_OBJECT

private:
	bool bIsHorizontal;
	bool bMoveInitiatedViaSetPosition;

protected:
	void initInstance();
	virtual QVariant itemChange(GraphicsItemChange oChange, const QVariant &oValue);

public:
	// main constructor
	explicit QtSssSiCcropLine(bool bHorizontal, QGraphicsItem *pParent = 0);
	// the other constructors are not yet coded
	explicit QtSssSiCcropLine(qreal fX1, qreal fY1, qreal fX2, qreal fY2, bool bHorizontal = true, QGraphicsItem *pParent = 0);
	explicit QtSssSiCcropLine(QGraphicsItem *pParent = 0);
	explicit QtSssSiCcropLine(const QLineF &oLine, QGraphicsItem *pParent = 0);
	explicit QtSssSiCcropLine(qreal fX1, qreal fY1, qreal fX2, qreal fY2, QGraphicsItem *pParent = 0);
	~QtSssSiCcropLine();

signals:
	void wasMoved();

public slots:
	void setPosition(qreal fXorY);

};



}	} // namespace SwissalpS::QtSssSiC



#endif // QTSSSSICCROPLINE_H
