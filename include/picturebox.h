#ifndef PICTUREBOX_H
#define PICTUREBOX_H

#include <QWidget>
#include <QImage>
#include <QPixmap>

enum PB_MODE {FIXED_SIZE, FIX_SIZE_CENTRED, AUTO_ZOOM, AUTO_SIZE, AUTO_FILL, TEXT_SHOW};

#define MAXTARGETNUM 32
typedef struct _VIDEO_FACE_POSITION_S//512
{
   	short FacePosX[MAXTARGETNUM];
	short FacePosY[MAXTARGETNUM];
	short FacePosW[MAXTARGETNUM];
	short FacePosH[MAXTARGETNUM];
	int   byTargetType[MAXTARGETNUM]; //0 »À¡≥£¨1 Õ∑ºÁ
	char  byRes[128];
}VIDEO_FACE_POSITION_S; 

class PictureBox : public QWidget
{
    Q_OBJECT
public:
    explicit PictureBox(QWidget *parent = 0);
    void setMode(PB_MODE mode);
    ~PictureBox();
	
	bool setImage2(QImage &image, double scale = 1.0);

private:
    QPixmap m_pixmap;
    double m_scale;
    PB_MODE m_mode;
    QBrush m_brush;

    PB_MODE m_mode_old;
    QString m_text;
	int FacePosX[MAXTARGETNUM];
	int FacePosY[MAXTARGETNUM];
	int FacePosW[MAXTARGETNUM];
	int FacePosH[MAXTARGETNUM];
	int mWithFacePos;
   // int m_text_x;
   // int m_text_y;
protected:
    void paintEvent(QPaintEvent * event);
signals:

public slots:
    bool setImage(QImage &image, double scale = 1.0);
    bool setImagewithFacePos(QImage &image, double scale = 1.0, VIDEO_FACE_POSITION_S *nFacePos = NULL);
    void setBackground(QBrush brush);
    void setTextInfo(QString strText, int bShow);
};

#endif // PICTUREBOX_H

