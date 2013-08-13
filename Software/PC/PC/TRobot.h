#pragma once
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>

class TRobot : public QGraphicsItem
{
public:
	TRobot(void);
	~TRobot(void);
	virtual QRectF boundingRect() const;
	virtual void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget);
	void SetSensors(int *sensors,int count);
protected: 
	int sonarlen_[8];
	void PaintSonar(int x,int y,int len,int deg,QPainter *painter);
	int GetSize(void) const;
	int GetDisdanceColor(int distance);
	
};

