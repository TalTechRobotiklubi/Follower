#include "TRobot.h"
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QtCore/qmath.h>


TRobot::TRobot(void)
{
	for (int i = 0; i< 8;i++)
	{
		sonarlen_[i] = -1;
	}
	zoom = 1;
}


TRobot::~TRobot(void)
{
}

QRectF TRobot::boundingRect() const
{
	return QRect(-GetSize()/2,-GetSize()/2,GetSize(),GetSize());
}


void TRobot::paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget)
{

	int sidelen = int (GetSize()/2.42);

	int s2 = sidelen>>1;
	int s3 =s2+int (sidelen*0.71);

	int s0 = -s3;
	int s1 = -s2;
	double s = zoom;

	QPen pen(Qt::gray);
	pen.setWidth(4);
	painter->setPen(pen);
	// draws octagon (main body of the robot)

	painter->drawLine(-14*s	,-20*s	,14*s	,-20*s	);
	painter->drawLine(-14*s	,-20*s	,-20*s	,-15*s	);
	painter->drawLine(-20*s	,-15*s	,-25*s	,0		);
	painter->drawLine(-25*s	,0		,-20*s	,15*s	);
	painter->drawLine(-20*s	,15*s	,-8*s	,23*s	);
	painter->drawLine(-8*s	,23*s	,8*s	,23*s	);
	painter->drawLine(8*s	,23*s	,20*s	,15*s	);
	painter->drawLine(20*s	,15*s	,25*s	,0		);
	painter->drawLine(25*s	,0		,20*s	,-15*s	);
	painter->drawLine(20*s	,-15*s	,14*s	,-20*s	);

	int s4 = s2+int (sidelen*0.355);

	// draws sonar signal 

PaintSonar(s3,0,sonarlen_[0],0,painter,s);
	PaintSensor(s3,0,0,painter,s,2);

	PaintSonar(s4,-s4,sonarlen_[1],45,painter,s);
	PaintSensor(s4,-s4,45,painter,s,2);

	PaintSonar(0,s0,sonarlen_[2],90,painter,s);
	PaintSensor(0,s0,90,painter,s,2);

	PaintSonar(-s4,-s4,sonarlen_[3],135,painter,s);
	PaintSensor(-s4,-s4,135,painter,s,2);

	PaintSonar(s0,0,sonarlen_[4],180,painter,s);
	PaintSensor(s0,0,180,painter,s,2);

	PaintSonar(-s4,s4,sonarlen_[5],225,painter,s);
	PaintSensor(-s4,s4,225,painter,s,2);

	PaintSonar(0,s3,sonarlen_[6],270,painter,s);
	PaintSensor(0,s3,270,painter,s,2);

	PaintSonar(s4,s4,sonarlen_[7],315,painter,s);
	PaintSensor(s4,s4,315,painter,s,2);
	


}
void TRobot::PaintSonar(int x,int y,int len,int deg,QPainter *painter,double size)
{	
	if ((len > 2000)|(len < 0)) return;

	QPen pen(Qt::blue);
	pen.setColor(GetDisdanceColor(len));
	pen.setWidth(3);
	painter->setPen(pen);

	for (int r = 0; r< len; r +=5 ){

		painter->drawArc(QRect((x-r)*size,(y-r)*size,(r<<1)*size,(r<<1)*size),((deg-15.5))*16,31*16);
	}
}

void TRobot::PaintSensor(int x,int y,int deg,QPainter *painter,double size,int state)
{
	QPen pen(Qt::blue);
	if (state){
		pen.setColor(Qt::red);
	}
	painter->setPen(pen);

	

	painter->drawEllipse((x-2)*size,(y-2)*size,4*size,4*size);

	if (size >3){
		QFont font;
		font.setPointSize(2*size);
		painter->setFont(font);
		painter->drawText((x-0.5)*size,(y+0.5)*size,"1");
	


		if (state == 2) {
			QPoint a[4];// = {6,0,2,0,5,-1,5,1};
			a[0] = RotatePoint(QPoint(6*size,0),deg);
			a[1] = RotatePoint(QPoint(2*size,0),deg);
			a[2] = RotatePoint(QPoint(5*size,-size),deg);
			a[3] = RotatePoint(QPoint(5*size,size),deg);
			int i = 0;

			for (i = 1; i<4;i++){
				painter->drawLine(x*size+a[i].x(),y*size+a[i].y(),x*size+a[0].x(),y*size+a[0].y());
			}

		}

	}

}

int TRobot::GetSize(void) const
{
	return 80;
}

int TRobot::GetDisdanceColor(int distance)
{

	unsigned char c = (distance & 0x1f)<<3;

	int i = distance>>5;

	if (i == 0){
		return 0xff0000;
	}else if (i == 1){
		return 0xff0000 | (c<<8);
	}else if (i == 2){
		c = ~c;
		return 0x00ff00 | (c<<16); 
	}else if (i == 3){
		 return 0x00ff00 | c;
	}else if (i == 4){
		c = ~c;
		return 0x0000ff | (c<<8); 
	}else{
		return 0x0000ff;
	}
}

void TRobot::SetSensors(int *sensors,int count)
{
	int i;
	int update = 0;

	if (sensors == NULL) 
		return;

	if (count > 8)
		count = 8;

	for (i = 0; i<count; i++)
	{
		if (*sensors != sonarlen_[i])
		{
			update = 1;
			sonarlen_[i] = *sensors;
		}
		sensors++;
	}

	if (update)
		this->update();
}

QPoint TRobot::RotatePoint(QPoint P,double degree)
{
	degree = 360-degree;
	degree *=   3.141592653 / 180;
	double s = qSin(degree);
	double c = qCos(degree);

	return QPoint(P.x()*c-P.y()*s,P.x()*s+P.y()*c);
}






