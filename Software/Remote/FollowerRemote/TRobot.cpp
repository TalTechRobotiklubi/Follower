#include "TRobot.h"
#include <QPainter>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsScene>
#include <QtCore/qmath.h>
#include <QtCore/QFile>


TRobot::TRobot(QSettings *settings)
{
    settings_ = settings;

	int sidelen = int (80/2.42);

	int s2 = sidelen>>1;
	int s3 =s2+int (sidelen*0.71);

	int s0 = -s3;
	int s4 = s2+int (sidelen*0.355);
	
    if (!LoadFromFile())
    {

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

	m1speed = 500;
	m2speed = 50;
}


TRobot::~TRobot(void)
{
}

QRectF TRobot::boundingRect() const
{
	return QRect(-GetSize()/2,-GetSize()/2,GetSize(),GetSize());
}


void TRobot::paint(QPainter *painter,const QStyleOptionGraphicsItem* ,QWidget*)
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


    int driveangle = 0;

    driveangle = GetSpeedAngle(m2speed,m1speed);

    PaintAngledArrow(0,0,-100,driveangle,painter,zoom);


}
void TRobot::PaintSonar(Tsensor *sen,QPainter *painter,double size)
{	
    if ((sen->sonarlen > 2000)||(sen->sonarlen < 0)) return;

	QPen pen(Qt::blue);
	pen.setColor(GetDisdanceColor(sen->sonarlen));
	pen.setWidth(3);
	painter->setPen(pen);

    for (int r = 0; r< sen->sonarlen; r +=5 )
    {
		painter->drawArc(QRect((sen->x-r)*size,(sen->y-r)*size,(r<<1)*size,(r<<1)*size),((sen->deg-15.5))*16,31*16);
	}
}

void TRobot::PaintSensor(Tsensor *sen,QPainter *painter,double size)
{
	QPen pen(Qt::blue);

    switch (sen->state)
    {
        case 1: pen.setColor(Qt::red); break;
        case 2: pen.setColor(Qt::yellow); break;
        default : pen.setColor(Qt::blue); break;
    }

	painter->setPen(pen);

	

	painter->drawEllipse((sen->x-2)*size,(sen->y-2)*size,4*size,4*size);

    if (size >3)
    {
		QFont font;
		font.setPointSize(2*size);
		painter->setFont(font);
        painter->drawText((sen->x-0.5)*size,(sen->y+0.5)*size,QString("%1").arg(sen->id));
	


        if (sen->state == 2)
        {
			QPoint a[4];// = {6,0,2,0,5,-1,5,1};
            a[0] = RotatePoint(QPoint(6*size,0),-sen->deg);
            a[1] = RotatePoint(QPoint(2*size,0),-sen->deg);
            a[2] = RotatePoint(QPoint(5*size,-size),-sen->deg);
            a[3] = RotatePoint(QPoint(5*size,size),-sen->deg);
			int i = 0;

            for (i = 1; i<4;i++)
            {
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

    if (i == 0)
    {
		return 0xff0000;
    }
    else if (i == 1)
    {
		return 0xff0000 | (c<<8);
    }
    else if (i == 2)
    {
		c = ~c;
		return 0x00ff00 | (c<<16); 
    }
    else if (i == 3)
    {
		 return 0x00ff00 | c;
    }
    else if (i == 4)
    {
		c = ~c;
		return 0x0000ff | (c<<8); 
    }
    else
    {
		return 0x0000ff;
	}
}

void TRobot::SetSensors(unsigned char *sensors,int count)
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

    while ((i<7) && !hassensor)
    {
		i++;
		hassensor = IsThereSonar(x,y,&sensor[i]);
	}

    if (hassensor)
    {
        if (sensor[i].state)
        {
			sensor[i].state = 0;
			SaveToFile();
        }
        else
        {
            int l = 0;
            int hasselected = sensor[0].state;

            while ((l<7) && !hasselected){
                l++;
                hasselected = sensor[l].state;
            }

            if (!hasselected)
            {
                if (button == Qt::LeftButton)
                {
                    sensor[i].state = 1;
                }
                else if (button == Qt::RightButton)
                {
                    sensor[i].state = 2;
                }
            }
		}

    }
    else
    {

        for (i = 0; i<8; i++)
        {
            if (sensor[i].state == 1)
            {
				sensor[i].x = x/zoom;
				sensor[i].y = y/zoom;
                sensor[i].state = 0;
			}

            if (sensor[i].state == 2)
            {
                sensor[i].deg = -90+GetAngle(x/zoom,y/zoom,sensor[i].x,sensor[i].y) % 360;
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
    return (int) (qAtan2(x1-x2,y1-y2)*57.29577951);
}

void TRobot::SaveToFile(void)
{
    settings_->beginGroup("trobot");
    settings_->beginWriteArray("sensor");

    for (int i = 0; i < 8; i++)
    {
        settings_->setArrayIndex(i);
        settings_->setValue("id", QString(sensor[i].id));
        settings_->setValue("x", sensor[i].x);
        settings_->setValue("y", sensor[i].y);
        settings_->setValue("deg", sensor[i].deg);
    }

    settings_->endArray();
    settings_->endGroup();
}

int TRobot::LoadFromFile(void)
{
    int size;

    settings_->beginGroup("trobot");
    size = settings_->beginReadArray("sensor");

    if (!size)
    {
        settings_->endArray();
        settings_->endGroup();

        return 0;
    }

    for (int i = 0; i < size; i++)
    {
        settings_->setArrayIndex(i);

        sensor[i].id = settings_->value("id").toChar().toLatin1();
        sensor[i].x = settings_->value("x").toInt();
        sensor[i].y = settings_->value("y").toInt();
        sensor[i].deg = settings_->value("deg").toInt();
    }

    settings_->endArray();
    settings_->endGroup();

    return 1;
}

double TRobot::GetSpeedAngle(double s1,double s2)
{
	return  ((s1-s2)/0.2)*0.017453;
}

void TRobot::PaintAngledArrow(int x1,int y1,int len,int angle,QPainter *painter,double size)
{
	int i;
	int x = x1;
	int y = y1;
	QPoint p;
	int segments = 5;
	int seglen = len / segments;
	int seg = angle/segments;

	QPen pen(Qt::green);
	pen.setWidth(2);
	painter->setPen(pen);

	p = QPoint(0,seglen);

	for (i = 0; i<segments; i++)
	{
		p = RotatePoint(p,seg);

		painter->drawLine(x*size,y*size,(x+p.x())*size,(y+p.y())*size);

		x += (int) p.x();
		y += (int) p.y();

	}
	p = RotatePoint(p,135);
	painter->drawLine(x*size,y*size,(x+p.x())*size,(y+p.y())*size);

	p = RotatePoint(p,90);
	painter->drawLine(x*size,y*size,(x+p.x())*size,(y+p.y())*size);
	
}






