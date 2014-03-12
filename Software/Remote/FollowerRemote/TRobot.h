#pragma once
#include <QPainter>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsScene>



class TRobot : public QGraphicsItem
{



public:
	TRobot(void);
	~TRobot(void);
	double zoom; 
	
	virtual QRectF boundingRect() const;
	virtual void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget);
	void SetSensors(unsigned char *sensors,int count);
	QPoint RotatePoint(QPoint P,double degree);
	void MousePressEvent(int x,int y,int button);
protected:
	struct Tsensor {
		int x;
		int y;
		int deg;
		int state;
		int sonarlen;
		char id;
		};
	Tsensor sensor[8];

	void PaintSonar(Tsensor *sen,QPainter *painter,double size);
	void PaintSensor(Tsensor *sen,QPainter *painter,double size);
	int IsThereSonar(int x,int y,Tsensor *sen);
	int GetSize(void) const;
	int GetDisdanceColor(int distance);
	int GetAngle(int x1,int y1,int x2,int y2);
	void SaveToFile(void);
	int TRobot::LoadFromFile(void);
	
};

