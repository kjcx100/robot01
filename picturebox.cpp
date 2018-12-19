#include "picturebox.h"
#include <QPainter>
#include <QDebug>
static const int IMAGE_WIDTH  = 160;
static const int IMAGE_HEIGHT = 120;
static const QSize IMAGE_SIZE = QSize(IMAGE_WIDTH, IMAGE_HEIGHT);

PictureBox::PictureBox(QWidget *parent) : QWidget(parent)
{
    m_pixmap = QPixmap(IMAGE_SIZE);
    m_pixmap.fill();
    m_scale = 1.0;
    m_mode = FIXED_SIZE;
    m_mode_old = m_mode;
    m_brush = QBrush(Qt::black);  //Qt::white
    for(int i=0;i<MAXTARGETNUM;i++)
    {
		FacePosX[i] = 0;
		FacePosY[i] = 0;
		FacePosW[i] = 0;
		FacePosH[i] = 0;
	}
	mWithFacePos = 0;
}

void PictureBox::setBackground(QBrush brush)
{
    m_brush = brush;
    update();
}

//void PictureBox::setTextInfo(int nX, int nY, QString strText, int bShow)
void PictureBox::setTextInfo(QString strText, int bShow)
{
    if(bShow == 1)
    {
        if(m_mode != TEXT_SHOW)
            m_mode_old = m_mode;
        m_mode = TEXT_SHOW;
        m_text.sprintf("%s", qPrintable(strText));
       // m_text_x = nX;
       // m_text_y = nY;
    }
    else
    {
        m_mode = m_mode_old;
    }
    update();
}


void PictureBox::setMode(PB_MODE mode)
{
    m_mode = mode;
    if(m_mode == AUTO_SIZE)
    {
        setFixedSize(m_pixmap.size() * m_scale);
    }
    else
    {
        setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        setMinimumSize(0, 0);
    }
    update();
}

bool PictureBox::setImage(QImage &image, double scale)
{
    if(image.isNull())
    {
        return false;
    }
	mWithFacePos = 0;
    m_pixmap = QPixmap::fromImage(image);
    m_scale = qBound(0.01, scale, 100.0);
    if(m_mode == AUTO_SIZE)
    {
        setFixedSize(m_pixmap.size() * m_scale);
    }
    update();
    return true;
}

bool PictureBox::setImagewithFacePos(QImage &image, double scale, VIDEO_FACE_POSITION_S *nFacePos)
{
    if(image.isNull())
    {
        return false;
    }
	mWithFacePos = 1;
    m_pixmap = QPixmap::fromImage(image);
    m_scale = qBound(0.01, scale, 100.0);
    if(m_mode == AUTO_SIZE)
    {
        setFixedSize(m_pixmap.size() * m_scale);
    }
	for(int i=0;i<MAXTARGETNUM;i++)
	{
		FacePosX[i] = nFacePos->FacePosX[i];
		FacePosY[i] = nFacePos->FacePosY[i];
		FacePosW[i] = nFacePos->FacePosW[i];
		FacePosH[i] = nFacePos->FacePosH[i];
	}
    update();
    return true;
}

bool PictureBox::setImage2(QImage &image, double scale)
{
    if(image.isNull())
    {
        return false;
    }
	mWithFacePos = 0;
    m_pixmap = QPixmap::fromImage(image);
    m_scale = qBound(0.01, scale, 100.0);
    if(m_mode == AUTO_SIZE)
    {
        setFixedSize(m_pixmap.size() * m_scale);
    }

	double window_width, window_height;
    double image_width, image_height;
    double r1, r2; // r;
   // int offset_x, offset_y;

	QPainter painter(this);
    //painter.setBackground(m_brush);
    //painter.eraseRect(rect());

	window_width = width();
	window_height = height();

	image_width = m_pixmap.width();
	image_height = m_pixmap.height();

	r1 = window_width  / image_width;
	r2 = window_height / image_height;

	//r = qMin(r1, r2);
	//offset_x = (window_width  - r * image_width)  / 2;
	//offset_y = (window_height - r * image_height) / 2;
	//painter.translate(offset_x, offset_y);

	painter.scale(r1, r2);
	painter.drawPixmap(0, 0, m_pixmap);

    return true;
}

void PictureBox::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setBackground(m_brush);
    painter.eraseRect(rect());

    double window_width, window_height;
    double image_width, image_height;
    double r1, r2, r;
    int offset_x, offset_y;

    switch (m_mode)
    {
    case FIXED_SIZE:
    case AUTO_SIZE:
        painter.scale(m_scale, m_scale);
        painter.drawPixmap(0, 0, m_pixmap);
        break;
    case FIX_SIZE_CENTRED:
        window_width = width();
        window_height = height();
        image_width = m_pixmap.width();
        image_height = m_pixmap.height();
        offset_x = (window_width - m_scale * image_width) / 2;
        offset_y = (window_height - m_scale * image_height) / 2;
        painter.translate(offset_x, offset_y);
        painter.scale(m_scale, m_scale);
        painter.drawPixmap(0, 0, m_pixmap);
        break;
    case AUTO_ZOOM:
        window_width = width();
        window_height = height();
        image_width = m_pixmap.width();
        image_height = m_pixmap.height();
        r1 = window_width / image_width;
        r2 = window_height / image_height;
        r = qMin(r1, r2);
        offset_x = (window_width - r * image_width) / 2;
        offset_y = (window_height - r * image_height) / 2;
        painter.translate(offset_x, offset_y);
        painter.scale(r, r);
        painter.drawPixmap(0, 0, m_pixmap);
        break;
     case AUTO_FILL:
        window_width = width();
        window_height = height();

        image_width = m_pixmap.width();
        image_height = m_pixmap.height();

        r1 = window_width  / image_width;
        r2 = window_height / image_height;

        //r = qMin(r1, r2);
        //offset_x = (window_width  - r * image_width)  / 2;
        //offset_y = (window_height - r * image_height) / 2;
        //painter.translate(offset_x, offset_y);

        painter.scale(r1, r2);
        painter.drawPixmap(0, 0, m_pixmap);
        
		if(mWithFacePos == 1)
		{
			painter.setPen(QPen(Qt::red,4));//设置画笔形式
			for(int i=0;i<MAXTARGETNUM;i++)
			{
            #if 0//矩形框
				painter.drawLine(FacePosX[i],FacePosY[i],FacePosX[i]+FacePosW[i],FacePosY[i]);//画横直线
			    painter.drawLine(FacePosX[i],FacePosY[i]+FacePosH[i],FacePosX[i]+FacePosW[i],FacePosY[i]+FacePosH[i]);
		    	painter.drawLine(FacePosX[i],FacePosY[i],FacePosX[i],FacePosY[i]+FacePosH[i]);//画竖直线
			    painter.drawLine(FacePosX[i]+FacePosW[i],FacePosY[i],FacePosX[i]+FacePosW[i],FacePosY[i]+FacePosH[i]);
			#else//矩形间隔框
				painter.drawLine(FacePosX[i],FacePosY[i],FacePosX[i]+FacePosW[i]/4,FacePosY[i]);//画横直线
				painter.drawLine(FacePosX[i]+FacePosW[i]*3/4,FacePosY[i],FacePosX[i]+FacePosW[i],FacePosY[i]);
				
			    painter.drawLine(FacePosX[i],FacePosY[i]+FacePosH[i],FacePosX[i]+FacePosW[i]/4,FacePosY[i]+FacePosH[i]);
				painter.drawLine(FacePosX[i]+FacePosW[i]*3/4,FacePosY[i]+FacePosH[i],FacePosX[i]+FacePosW[i],FacePosY[i]+FacePosH[i]);

				painter.drawLine(FacePosX[i],FacePosY[i],FacePosX[i],FacePosY[i]+FacePosH[i]/4);//画竖直线
				painter.drawLine(FacePosX[i],FacePosY[i]+FacePosH[i]*3/4,FacePosX[i],FacePosY[i]+FacePosH[i]);
				
			    painter.drawLine(FacePosX[i]+FacePosW[i],FacePosY[i],FacePosX[i]+FacePosW[i],FacePosY[i]+FacePosH[i]/4);	
				painter.drawLine(FacePosX[i]+FacePosW[i],FacePosY[i]+FacePosH[i]*3/4,FacePosX[i]+FacePosW[i],FacePosY[i]+FacePosH[i]);	
			#endif
            }
			mWithFacePos = 0;
		}
        
        break;
    case TEXT_SHOW:
        //painter.drawText(QPoint(m_text_x,m_text_y),m_text);
        
        //QPen pen = painter.pen();
       // QPen pen;
        //pen.setColor(Qt::red);
        painter.setPen(QColor(255, 0, 0));
        
        //QFont font = painter.font();
        QFont font;
        //font.setBold(true);//加粗
        font.setPixelSize(30);//改变字体大小

       // painter.setPen(pen);
        painter.setFont(font);
        //painter.setFont(QFont("宋体",50,QFont::Bold));

        painter.drawText(rect(),Qt::AlignCenter,m_text);
        break; 
    }
}

PictureBox::~PictureBox()
{

}
