#include "mainwindow.h"
#include <QApplication>
#include "define.h"
#include "include/image_process.h"

//ImageProcessThread m_ImageProcessThread;
//ImageDispThread m_ImageDispThread;
BTCommunThread m_BTCommunThread;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug("main start!!!%d\n",__LINE__);
    MainWindow w;
    w.show();
    qDebug("main start!!!%d\n",__LINE__);
    m_BTCommunThread.start();

    //m_ImageProcessThread.start();
    //m_ImageDispThread.start();
    //MY_SLEEP_NS(5);

    return a.exec();
}


