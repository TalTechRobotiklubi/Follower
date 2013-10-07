#include "TRobot.h"
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QtCore/qmath.h>
#include <QtCore/QFile>


TRobot::TRobot(void)
{

	int sidelen = int (80/2.42);

	int s2 = sidelen>>1;
	int s3 =s2+int (sidelen*0.71);

	int s0 = -s3;
	int s1 = -s2;
	double s = zoom;
	int s4 = s2+int (sidelen*0.355);
	
	if (!LoadFromFile()){

		sensor[0].x = s3;
		sensor[0].y = 0;

		sensor[1].x = s4;
		sensor[1].y = -s4;

		sensor[2].x = 0;
		sensor[2].y = s0;

		sensor[3].x = -s4;
		sensor[3].y = -s4;

		sensor[4].x = s0;
		sensor[4].y = 0;

		sensor[5].x = -s4;
		sensor[5].y = s4;

		sensor[6].x = 0;
		sensor[6].y = s3;

		sensor[7].x = s4;
		sensor[7].y = s4;


		for (int i = 0; i< 8;i++)
		{
			sensor[i].sonarlen = -1;
			sensor[i].state = 0;
			sensor[i].deg = 45*i;
			sensor[i].id = '1'+i;

		}
		

		SaveToFile();
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
	int i;

	QPen pen(Qt::gray);
	pen.setWidth(4);
	painter->setPen(pen);
	// draws octagon (main body of the robot)

	painter->drawLine(-14*zoom	,-20*zoom	,14*zoom	,-20*zoom);
	painter->drawLine(-14*zoom	,-20*zoom	,-20*zoom	,-15*zoom);
	painter->drawLine(-20*zoom	,-15*zoom	,-25*zoom	,0		);
	painter->drawLine(-25*zoom	,0			,-20*zoom	,15*zoom);
	painter->drawLine(-20*zoom	,15*zoom	,-8*zoom	,23*zoom);
	painter->drawLine(-8*zoom	,23*zoom	,8*zoom		,23*zoom);
	painter->drawLine(8*zoom	,23*zoom	,20*zoom	,15*zoom);
	painter->drawLine(20*zoom	,15*zoom	,25*zoom	,0		);
	painter->drawLine(25*zoom	,0			,20*zoom	,-15*zoom);
	painter->drawLine(20*zoom	,-15*zoom	,14*zoom	,-20*zoom);

	// draws sonar signal 
	for (i = 0; i< 8;i++)
	{
		PaintSensor(&sensor[i],painter,zoom);
		PaintSonar(&sensor[i],painter,zoom);
	}



}
void TRobot::PaintSonar(Tsensor *sen,QPainter *painter,double size)
{	
	if ((sen->sonarlen > 2000)|(sen->sonarlen < 0)) return;

	QPen pen(Qt::blue);
	pen.setColor(GetDisdanceColor(sen->sonarlen));
	pen.setWidth(3);
	painter->setPen(pen);

	for (int r = 0; r< sen->sonarlen; r +=5 ){

		painter->drawArc(QRect((sen->x-r)*size,(sen->y-r)*size,(r<<1)*size,(r<<1)*size),((sen->deg-15.5))*16,31*16);
	}
}

void TRobot::PaintSensor(Tsensor *sen,QPainter *painter,double size)
{
	QPen pen(Qt::blue);
	if (sen->state){
		pen.setColor(Qt::red);
	}
	painter->setPen(pen);

	

	painter->drawEllipse((sen->x-2)*size,(sen->y-2)*size,4*size,4*size);

	if (size >3){
		QFont font;
		font.setPointSize(2*size);
		painter->setFont(font);
		painter->drawText((sen->x-0.5)*size,(sen->y+0.5)*size,QString("%1").arg(sen->id));
	


		if (sen->state == 2) {
			QPoint a[4];// = {6,0,2,0,5,-1,5,1};
			a[0] = RotatePoint(QPoint(6*size,0),sen->deg);
			a[1] = RotatePoint(QPoint(2*size,0),sen->deg);
			a[2] = RotatePoint(QPoint(5*size,-size),sen->deg);
			a[3] = RotatePoint(QPoint(5*size,size),sen->deg);
			int i = 0;

			for (i = 1; i<4;i++){
				painter->drawLine(sen->x*size+a[i].x(),sen->y*size+a[i].y(),sen->x*size+a[0].x(),sen->y*size+a[0].y());
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
		if (*sensors != sensor[i].sonarlen)
		{
			update = 1;
			sensor[i].sonarlen = *sensors;
		}
		sensors++;
	}

	if (update)
		this->update();
}

QPoint TRobot::RotatePoint(QPoint P,double degree)
{
	//degree = 360-degree;
	degree *=   3.141592653 / 180;
	double s = qSin(degree);
	double c = qCos(degree);

	return QPoint(P.x()*c-P.y()*s,P.x()*s+P.y()*c);
}

void TRobot::MousePressEvent(int x,int y,int button)
{
	int i = -1;
	int hassensor = 0;

	while ((i<8) && !hassensor){
		i++;
		hassensor = IsThereSonar(x,y,&sensor[i]);
	}

	if (hassensor){
		if (sensor[i].state){
			sensor[i].state = 0;
			SaveToFile();
		}else{
			if (button == Qt::LeftButton){
				sensor[i].state = 1;
			}else if (button == Qt::RightButton){
				sensor[i].state = 2;
			}
		}

	}else{

		for (i = 0; i<8; i++){
			if (sensor[i].state == 1){
				sensor[i].x = x/zoom;
				sensor[i].y = y/zoom;
			}

			if (sensor[i].state == 2){
				sensor[i].deg = GetAngle(sensor[i].x,sensor[i].y,x/zoom,y/zoom);
			}


		}

	}

}

int TRobot::IsThereSonar(int x,int y,Tsensor *sen)
{
	return (x>(sen->x-2)*zoom)&&(x<(sen->x+2)*zoom)&&(y>(sen->y-2)*zoom)&&(y<(sen->y+2)*zoom);
}


int TRobot::GetAngle(int x1,int y1,int x2,int y2)
{


	if (y1 == y2){
		if (x1 < x2){
			return 0;
		}else if (x1 > x2){
			return 180;
		}
	}

	if ((x1==x2)&&(y1==y2)){
		return 0;
	}

	double part1,part2;

	part1 = abs(y2-y1);
	if (part1==0) {
		part1 = 0.0000001;
	}

	part2 = abs(x2-x1);
	if (part2==0){
		part2 = 0.0000001;
	}

	int angle = (int)(qAtan(part1/part2)*57.29577951);

	if ((x1>x2) && (y1<y2)) angle = 180-angle;
	if ((x1>x2) && (y1>y2)) angle = angle +180;
	if ((x1<x2) && (y1>y2)) angle = 360-angle;

	return angle;
}

void TRobot::SaveToFile(void)
{

	QFile file("robot.txt");
    if (file.open(QIODevice::WriteOnly))
	{
		QDataStream out(&file); 

		for (int i = 0; i< 8; i++)
		{
			out << sensor[i].x<<sensor[i].y<<sensor[i].deg; 
		}
		file.close();
	}
}

int TRobot::LoadFromFile(void)
{

	QFile file("robot.txt");
    if (file.open(QIODevice::ReadOnly))
    {
		QDataStream in(&file); 

		for (int i = 0; i< 8; i++)
		{
			in >> sensor[i].x; 
			in >> sensor[i].y; 
			in >> sensor[i].deg; 
			sensor[i].id = '0'+i;
			sensor[i].sonarlen = -1;
			sensor[i].state = 0;
		}

		file.close();
		return 1;
	}
	return 0;
}






