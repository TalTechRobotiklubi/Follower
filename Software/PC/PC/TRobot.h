#pragma once
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>

class TRobot : public QGraphicsItem
{
public:
	

	TRobot(void);
	~TRobot(void);
	QRectF boundingRect() const;
	void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget);
	void SetSensors(int *sensors,int count);
protected: 
	int sonarlen[8];
	void PaintSonar(int x,int y,int len,int deg,QPainter *painter);
	int getsize(void) const;
	int GetDisdanceColor(int distance);
	
};

