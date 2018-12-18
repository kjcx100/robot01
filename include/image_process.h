#ifndef IMAGE_PROCESS_H
#define IMAGE_PROCESS_H

#include <QThread>
#include <functional>
#include <QtCore/QThread>
#include <QWaitCondition>

#include "define.h"

void MY_SLEEP_MS(int ms);
void MY_SLEEP_NS(int s);

class BTCommunThread : public QThread
{
    Q_OBJECT
public:
    bool bStop ;
    explicit BTCommunThread(QObject *parent = 0);
    void run();
    void stop();
    //int CheckRFCard(unsigned char *pCardSN, unsigned char SNLen);

signals:


};
class ImageProcessThread : public QThread
{
    Q_OBJECT

public:
    bool bStop ;
    explicit ImageProcessThread(QObject *parent = 0);
    void run();
    void stop();
};
#endif // IMAGE_PROCESS_H
