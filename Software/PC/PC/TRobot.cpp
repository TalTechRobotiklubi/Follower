#include "TRobot.h"
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>


TRobot::TRobot(void)
{
	for (int i = 0; i< 8;i++)
	{
		sonarlen[i] = -1;
	}
}


TRobot::~TRobot(void)
{
}

QRectF TRobot::boundingRect() const
{
	return QRect(-getsize()/2,-getsize()/2,getsize(),getsize());
}


void TRobot::paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget)
{

	int sidelen = int (getsize()/2.42);

	int s2 = sidelen>>1;
	int s3 =s2+int (sidelen*0.71);

	int s0 = -s3;
	int s1 = -s2;


	QPen pen(Qt::gray);
	pen.setWidth(4);
	painter->setPen(pen);
	// draws octagon (main body of the robot)

	painter->drawLine(s1,s0,s2,s0);
	painter->drawLine(s2,s0,s3,s1);
	painter->drawLine(s3,s1,s3,s2);
	painter->drawLine(s3,s2,s2,s3);
	painter->drawLine(s2,s3,s1,s3);
	painter->drawLine(s1,s3,s0,s2);
	painter->drawLine(s0,s2,s0,s1);
	painter->drawLine(s0,s1,s1,s0);

	int s4 = s2+int (sidelen*0.355);

	// draws sonar signal 

	PaintSonar(s3,0,sonarlen[0],0,painter);
	PaintSonar(s4,-s4,sonarlen[1],45,painter);
	PaintSonar(0,s0,sonarlen[2],90,painter);
	PaintSonar(-s4,-s4,sonarlen[3],135,painter);
	PaintSonar(s0,0,sonarlen[4],180,painter);
	PaintSonar(-s4,s4,sonarlen[5],225,painter);
	PaintSonar(0,s3,sonarlen[6],270,painter);
	PaintSonar(s4,s4,sonarlen[7],315,painter);
	


}
void TRobot::PaintSonar(int x,int y,int len,int deg,QPainter *painter)
{	
	if ((len > 2000)|(len < 0)) return;

	QPen pen(Qt::blue);
	pen.setColor(GetDisdanceColor(len));
	pen.setWidth(3);
	painter->setPen(pen);

	for (int r = 0; r< len; r +=5 ){

		painter->drawArc(QRect(x-r,y-r,r<<1,r<<1),((deg-30))*16,60*16);
	}
}

int TRobot::getsize(void) const
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
		if (*sensors != sonarlen[i])
		{
			update = 1;
			sonarlen[i] = *sensors;
		}
		sensors++;
	}

	if (update)
		this->update();
}






