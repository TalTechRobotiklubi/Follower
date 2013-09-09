#pragma once
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>

class TRobot : public QGraphicsItem
{
public:
	TRobot(void);
	~TRobot(void);
	double zoom; 
	virtual QRectF boundingRect() const;
	virtual void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget);
	void SetSensors(int *sensors,int count);
	QPoint RotatePoint(QPoint P,double degree);
protected: 
	int sonarlen_[8];
	void PaintSonar(int x,int y,int len,int deg,QPainter *painter,double size);
	void PaintSensor(int x,int y,int deg,QPainter *painter,double size,int state);
	int GetSize(void) const;
	int GetDisdanceColor(int distance);
	
};

