/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *RawImg;
    QLabel *OutImg;
    QLabel *ApyToCam;
    QLabel *ApyToCam_2;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_5;
    QLabel *ApyToCam_4;
    QLineEdit *EditVer_View;
    QLabel *ApyToCam_5;
    QLineEdit *PixWidth_Start;
    QLineEdit *PixWidth_End;
    QLineEdit *PixHight_End;
    QLineEdit *PixHight_Start;
    QLabel *ApyToCam_6;
    QWidget *layoutWidget_4;
    QHBoxLayout *horizontalLayout_8;
    QLabel *ApyToCam_9;
    QLineEdit *EditHor_Angl;
    QWidget *layoutWidget_5;
    QHBoxLayout *horizontalLayout_9;
    QLabel *ApyToCam_10;
    QLineEdit *EditVer_Angl;
    QPushButton *TempBtn;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *Comb_Cam;
    QWidget *layoutWidget2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *ApyToCam_3;
    QLineEdit *EditHor_View;
    QCheckBox *ApyCam1;
    QCheckBox *ApyCam2;
    QCheckBox *ApyCam3;
    QCheckBox *ApyCam4;
    QCheckBox *ApyCam5;
    QLabel *ApyToCam_8;
    QLabel *ApyToCam_14;
    QLineEdit *GussBlurSize;
    QLabel *ApyToCam_11;
    QLabel *ApyToCam_16;
    QLineEdit *MidBlurSize;
    QLabel *ApyToCam_17;
    QLineEdit *calib_horizon;
    QLabel *ApyToCam_18;
    QLabel *ApyToCam_19;
    QLabel *ApyToCam_21;
    QLineEdit *calib_vertical;
    QLabel *ApyToCam_20;
    QLabel *ApyToCam_22;
    QLineEdit *GussBlurSize_2;
    QLabel *ApyToCam_24;
    QLabel *ApyToCam_25;
    QLineEdit *Gain_Fre;
    QLabel *ApyToCam_13;
    QLabel *ApyToCam_27;
    QLabel *ApyToCam_28;
    QLineEdit *calib_horizon_2;
    QLabel *ApyToCam_29;
    QLineEdit *calib_vertical_2;
    QLineEdit *Gain_thold_max;
    QLabel *ApyToCam_30;
    QLineEdit *Gain_thold_min;
    QLabel *ApyToCam_31;
    QLabel *ApyToCam_32;
    QPushButton *SaveBtn;
    QLabel *ApyToCam_12;
    QLabel *ApyToCam_15;
    QMenuBar *menuBar;
    QMenu *menuCamSet;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1280, 720);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        RawImg = new QLabel(centralWidget);
        RawImg->setObjectName(QStringLiteral("RawImg"));
        RawImg->setGeometry(QRect(480, 60, 320, 240));
        RawImg->setPixmap(QPixmap(QString::fromUtf8(":/bg_img/robot_bgimg.jpg")));
        OutImg = new QLabel(centralWidget);
        OutImg->setObjectName(QStringLiteral("OutImg"));
        OutImg->setGeometry(QRect(480, 340, 320, 240));
        OutImg->setPixmap(QPixmap(QString::fromUtf8(":/bg_img/robot_bgimg.jpg")));
        ApyToCam = new QLabel(centralWidget);
        ApyToCam->setObjectName(QStringLiteral("ApyToCam"));
        ApyToCam->setGeometry(QRect(20, 80, 60, 25));
        ApyToCam_2 = new QLabel(centralWidget);
        ApyToCam_2->setObjectName(QStringLiteral("ApyToCam_2"));
        ApyToCam_2->setGeometry(QRect(20, 110, 60, 25));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(230, 110, 131, 27));
        horizontalLayout_5 = new QHBoxLayout(layoutWidget);
        horizontalLayout_5->setSpacing(10);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        ApyToCam_4 = new QLabel(layoutWidget);
        ApyToCam_4->setObjectName(QStringLiteral("ApyToCam_4"));

        horizontalLayout_5->addWidget(ApyToCam_4);

        EditVer_View = new QLineEdit(layoutWidget);
        EditVer_View->setObjectName(QStringLiteral("EditVer_View"));
        EditVer_View->setEnabled(true);

        horizontalLayout_5->addWidget(EditVer_View);

        ApyToCam_5 = new QLabel(centralWidget);
        ApyToCam_5->setObjectName(QStringLiteral("ApyToCam_5"));
        ApyToCam_5->setGeometry(QRect(90, 150, 90, 25));
        PixWidth_Start = new QLineEdit(centralWidget);
        PixWidth_Start->setObjectName(QStringLiteral("PixWidth_Start"));
        PixWidth_Start->setEnabled(true);
        PixWidth_Start->setGeometry(QRect(190, 150, 59, 25));
        PixWidth_End = new QLineEdit(centralWidget);
        PixWidth_End->setObjectName(QStringLiteral("PixWidth_End"));
        PixWidth_End->setEnabled(true);
        PixWidth_End->setGeometry(QRect(260, 150, 59, 25));
        PixHight_End = new QLineEdit(centralWidget);
        PixHight_End->setObjectName(QStringLiteral("PixHight_End"));
        PixHight_End->setEnabled(true);
        PixHight_End->setGeometry(QRect(260, 180, 59, 25));
        PixHight_Start = new QLineEdit(centralWidget);
        PixHight_Start->setObjectName(QStringLiteral("PixHight_Start"));
        PixHight_Start->setEnabled(true);
        PixHight_Start->setGeometry(QRect(190, 180, 59, 25));
        ApyToCam_6 = new QLabel(centralWidget);
        ApyToCam_6->setObjectName(QStringLiteral("ApyToCam_6"));
        ApyToCam_6->setGeometry(QRect(90, 180, 90, 25));
        layoutWidget_4 = new QWidget(centralWidget);
        layoutWidget_4->setObjectName(QStringLiteral("layoutWidget_4"));
        layoutWidget_4->setGeometry(QRect(90, 210, 131, 27));
        horizontalLayout_8 = new QHBoxLayout(layoutWidget_4);
        horizontalLayout_8->setSpacing(10);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(0, 0, 0, 0);
        ApyToCam_9 = new QLabel(layoutWidget_4);
        ApyToCam_9->setObjectName(QStringLiteral("ApyToCam_9"));

        horizontalLayout_8->addWidget(ApyToCam_9);

        EditHor_Angl = new QLineEdit(layoutWidget_4);
        EditHor_Angl->setObjectName(QStringLiteral("EditHor_Angl"));
        EditHor_Angl->setEnabled(true);

        horizontalLayout_8->addWidget(EditHor_Angl);

        layoutWidget_5 = new QWidget(centralWidget);
        layoutWidget_5->setObjectName(QStringLiteral("layoutWidget_5"));
        layoutWidget_5->setGeometry(QRect(230, 210, 131, 27));
        horizontalLayout_9 = new QHBoxLayout(layoutWidget_5);
        horizontalLayout_9->setSpacing(10);
        horizontalLayout_9->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        horizontalLayout_9->setContentsMargins(0, 0, 0, 0);
        ApyToCam_10 = new QLabel(layoutWidget_5);
        ApyToCam_10->setObjectName(QStringLiteral("ApyToCam_10"));

        horizontalLayout_9->addWidget(ApyToCam_10);

        EditVer_Angl = new QLineEdit(layoutWidget_5);
        EditVer_Angl->setObjectName(QStringLiteral("EditVer_Angl"));
        EditVer_Angl->setEnabled(true);

        horizontalLayout_9->addWidget(EditVer_Angl);

        TempBtn = new QPushButton(centralWidget);
        TempBtn->setObjectName(QStringLiteral("TempBtn"));
        TempBtn->setGeometry(QRect(220, 35, 90, 25));
        layoutWidget1 = new QWidget(centralWidget);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(20, 30, 162, 35));
        horizontalLayout = new QHBoxLayout(layoutWidget1);
        horizontalLayout->setSpacing(10);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 5, 5, 5);
        label = new QLabel(layoutWidget1);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        Comb_Cam = new QComboBox(layoutWidget1);
        Comb_Cam->addItem(QString());
        Comb_Cam->addItem(QString());
        Comb_Cam->addItem(QString());
        Comb_Cam->addItem(QString());
        Comb_Cam->setObjectName(QStringLiteral("Comb_Cam"));

        horizontalLayout->addWidget(Comb_Cam);

        layoutWidget2 = new QWidget(centralWidget);
        layoutWidget2->setObjectName(QStringLiteral("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(90, 110, 131, 27));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget2);
        horizontalLayout_4->setSpacing(10);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        ApyToCam_3 = new QLabel(layoutWidget2);
        ApyToCam_3->setObjectName(QStringLiteral("ApyToCam_3"));

        horizontalLayout_4->addWidget(ApyToCam_3);

        EditHor_View = new QLineEdit(layoutWidget2);
        EditHor_View->setObjectName(QStringLiteral("EditHor_View"));
        EditHor_View->setEnabled(true);

        horizontalLayout_4->addWidget(EditHor_View);

        ApyCam1 = new QCheckBox(centralWidget);
        ApyCam1->setObjectName(QStringLiteral("ApyCam1"));
        ApyCam1->setGeometry(QRect(100, 80, 61, 23));
        ApyCam2 = new QCheckBox(centralWidget);
        ApyCam2->setObjectName(QStringLiteral("ApyCam2"));
        ApyCam2->setGeometry(QRect(160, 80, 61, 23));
        ApyCam3 = new QCheckBox(centralWidget);
        ApyCam3->setObjectName(QStringLiteral("ApyCam3"));
        ApyCam3->setGeometry(QRect(220, 80, 61, 23));
        ApyCam4 = new QCheckBox(centralWidget);
        ApyCam4->setObjectName(QStringLiteral("ApyCam4"));
        ApyCam4->setGeometry(QRect(340, 80, 61, 23));
        ApyCam5 = new QCheckBox(centralWidget);
        ApyCam5->setObjectName(QStringLiteral("ApyCam5"));
        ApyCam5->setGeometry(QRect(280, 80, 61, 23));
        ApyToCam_8 = new QLabel(centralWidget);
        ApyToCam_8->setObjectName(QStringLiteral("ApyToCam_8"));
        ApyToCam_8->setGeometry(QRect(20, 250, 60, 25));
        ApyToCam_14 = new QLabel(centralWidget);
        ApyToCam_14->setObjectName(QStringLiteral("ApyToCam_14"));
        ApyToCam_14->setGeometry(QRect(90, 270, 90, 25));
        GussBlurSize = new QLineEdit(centralWidget);
        GussBlurSize->setObjectName(QStringLiteral("GussBlurSize"));
        GussBlurSize->setEnabled(true);
        GussBlurSize->setGeometry(QRect(300, 270, 59, 25));
        ApyToCam_11 = new QLabel(centralWidget);
        ApyToCam_11->setObjectName(QStringLiteral("ApyToCam_11"));
        ApyToCam_11->setGeometry(QRect(250, 270, 51, 25));
        ApyToCam_16 = new QLabel(centralWidget);
        ApyToCam_16->setObjectName(QStringLiteral("ApyToCam_16"));
        ApyToCam_16->setGeometry(QRect(90, 300, 91, 25));
        MidBlurSize = new QLineEdit(centralWidget);
        MidBlurSize->setObjectName(QStringLiteral("MidBlurSize"));
        MidBlurSize->setEnabled(true);
        MidBlurSize->setGeometry(QRect(300, 300, 59, 25));
        ApyToCam_17 = new QLabel(centralWidget);
        ApyToCam_17->setObjectName(QStringLiteral("ApyToCam_17"));
        ApyToCam_17->setGeometry(QRect(250, 300, 51, 25));
        calib_horizon = new QLineEdit(centralWidget);
        calib_horizon->setObjectName(QStringLiteral("calib_horizon"));
        calib_horizon->setEnabled(true);
        calib_horizon->setGeometry(QRect(258, 330, 40, 25));
        ApyToCam_18 = new QLabel(centralWidget);
        ApyToCam_18->setObjectName(QStringLiteral("ApyToCam_18"));
        ApyToCam_18->setGeometry(QRect(190, 330, 62, 25));
        ApyToCam_19 = new QLabel(centralWidget);
        ApyToCam_19->setObjectName(QStringLiteral("ApyToCam_19"));
        ApyToCam_19->setGeometry(QRect(90, 330, 91, 25));
        ApyToCam_21 = new QLabel(centralWidget);
        ApyToCam_21->setObjectName(QStringLiteral("ApyToCam_21"));
        ApyToCam_21->setGeometry(QRect(310, 330, 62, 25));
        calib_vertical = new QLineEdit(centralWidget);
        calib_vertical->setObjectName(QStringLiteral("calib_vertical"));
        calib_vertical->setEnabled(true);
        calib_vertical->setGeometry(QRect(380, 330, 40, 25));
        ApyToCam_20 = new QLabel(centralWidget);
        ApyToCam_20->setObjectName(QStringLiteral("ApyToCam_20"));
        ApyToCam_20->setGeometry(QRect(90, 360, 90, 25));
        ApyToCam_22 = new QLabel(centralWidget);
        ApyToCam_22->setObjectName(QStringLiteral("ApyToCam_22"));
        ApyToCam_22->setGeometry(QRect(250, 360, 51, 25));
        GussBlurSize_2 = new QLineEdit(centralWidget);
        GussBlurSize_2->setObjectName(QStringLiteral("GussBlurSize_2"));
        GussBlurSize_2->setEnabled(true);
        GussBlurSize_2->setGeometry(QRect(300, 360, 59, 25));
        ApyToCam_24 = new QLabel(centralWidget);
        ApyToCam_24->setObjectName(QStringLiteral("ApyToCam_24"));
        ApyToCam_24->setGeometry(QRect(190, 460, 50, 25));
        ApyToCam_25 = new QLabel(centralWidget);
        ApyToCam_25->setObjectName(QStringLiteral("ApyToCam_25"));
        ApyToCam_25->setGeometry(QRect(90, 460, 91, 25));
        Gain_Fre = new QLineEdit(centralWidget);
        Gain_Fre->setObjectName(QStringLiteral("Gain_Fre"));
        Gain_Fre->setEnabled(true);
        Gain_Fre->setGeometry(QRect(240, 430, 59, 25));
        ApyToCam_13 = new QLabel(centralWidget);
        ApyToCam_13->setObjectName(QStringLiteral("ApyToCam_13"));
        ApyToCam_13->setGeometry(QRect(20, 380, 60, 25));
        ApyToCam_27 = new QLabel(centralWidget);
        ApyToCam_27->setObjectName(QStringLiteral("ApyToCam_27"));
        ApyToCam_27->setGeometry(QRect(190, 430, 51, 25));
        ApyToCam_28 = new QLabel(centralWidget);
        ApyToCam_28->setObjectName(QStringLiteral("ApyToCam_28"));
        ApyToCam_28->setGeometry(QRect(90, 430, 91, 25));
        calib_horizon_2 = new QLineEdit(centralWidget);
        calib_horizon_2->setObjectName(QStringLiteral("calib_horizon_2"));
        calib_horizon_2->setEnabled(true);
        calib_horizon_2->setGeometry(QRect(240, 460, 50, 25));
        ApyToCam_29 = new QLabel(centralWidget);
        ApyToCam_29->setObjectName(QStringLiteral("ApyToCam_29"));
        ApyToCam_29->setGeometry(QRect(310, 460, 50, 25));
        calib_vertical_2 = new QLineEdit(centralWidget);
        calib_vertical_2->setObjectName(QStringLiteral("calib_vertical_2"));
        calib_vertical_2->setEnabled(true);
        calib_vertical_2->setGeometry(QRect(360, 460, 50, 25));
        Gain_thold_max = new QLineEdit(centralWidget);
        Gain_thold_max->setObjectName(QStringLiteral("Gain_thold_max"));
        Gain_thold_max->setEnabled(true);
        Gain_thold_max->setGeometry(QRect(360, 400, 50, 25));
        ApyToCam_30 = new QLabel(centralWidget);
        ApyToCam_30->setObjectName(QStringLiteral("ApyToCam_30"));
        ApyToCam_30->setGeometry(QRect(310, 400, 50, 25));
        Gain_thold_min = new QLineEdit(centralWidget);
        Gain_thold_min->setObjectName(QStringLiteral("Gain_thold_min"));
        Gain_thold_min->setEnabled(true);
        Gain_thold_min->setGeometry(QRect(240, 400, 50, 25));
        ApyToCam_31 = new QLabel(centralWidget);
        ApyToCam_31->setObjectName(QStringLiteral("ApyToCam_31"));
        ApyToCam_31->setGeometry(QRect(90, 400, 91, 25));
        ApyToCam_32 = new QLabel(centralWidget);
        ApyToCam_32->setObjectName(QStringLiteral("ApyToCam_32"));
        ApyToCam_32->setGeometry(QRect(190, 400, 50, 25));
        SaveBtn = new QPushButton(centralWidget);
        SaveBtn->setObjectName(QStringLiteral("SaveBtn"));
        SaveBtn->setGeometry(QRect(90, 520, 90, 25));
        ApyToCam_12 = new QLabel(centralWidget);
        ApyToCam_12->setObjectName(QStringLiteral("ApyToCam_12"));
        ApyToCam_12->setGeometry(QRect(480, 30, 60, 25));
        ApyToCam_15 = new QLabel(centralWidget);
        ApyToCam_15->setObjectName(QStringLiteral("ApyToCam_15"));
        ApyToCam_15->setGeometry(QRect(480, 310, 60, 25));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1280, 28));
        menuCamSet = new QMenu(menuBar);
        menuCamSet->setObjectName(QStringLiteral("menuCamSet"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuCamSet->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        RawImg->setText(QString());
        OutImg->setText(QString());
        ApyToCam->setText(QApplication::translate("MainWindow", "\350\276\223\345\207\272\351\205\215\347\275\256", nullptr));
        ApyToCam_2->setText(QApplication::translate("MainWindow", "\345\256\211\350\243\205\345\217\202\346\225\260", nullptr));
        ApyToCam_4->setText(QApplication::translate("MainWindow", "\345\236\202\347\233\264\350\247\206\345\234\272", nullptr));
        ApyToCam_5->setText(QApplication::translate("MainWindow", "\346\260\264\345\271\263\350\243\201\345\211\252\350\214\203\345\233\264", nullptr));
        ApyToCam_6->setText(QApplication::translate("MainWindow", "\345\236\202\347\233\264\350\243\201\345\211\252\350\214\203\345\233\264", nullptr));
        ApyToCam_9->setText(QApplication::translate("MainWindow", "\346\260\264\345\271\263\350\247\222\345\272\246", nullptr));
        ApyToCam_10->setText(QApplication::translate("MainWindow", "\345\236\202\347\233\264\350\247\222\345\272\246", nullptr));
        TempBtn->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\346\250\241\346\235\277", nullptr));
        label->setText(QApplication::translate("MainWindow", "\351\200\211\346\213\251\347\233\270\346\234\272", nullptr));
        Comb_Cam->setItemText(0, QApplication::translate("MainWindow", "cama01", nullptr));
        Comb_Cam->setItemText(1, QApplication::translate("MainWindow", "cama02", nullptr));
        Comb_Cam->setItemText(2, QApplication::translate("MainWindow", "cama03", nullptr));
        Comb_Cam->setItemText(3, QApplication::translate("MainWindow", "cama04", nullptr));

        ApyToCam_3->setText(QApplication::translate("MainWindow", "\346\260\264\345\271\263\350\247\206\345\234\272", nullptr));
        ApyCam1->setText(QApplication::translate("MainWindow", "Cam1", nullptr));
        ApyCam2->setText(QApplication::translate("MainWindow", "Cam2", nullptr));
        ApyCam3->setText(QApplication::translate("MainWindow", "Cam3", nullptr));
        ApyCam4->setText(QApplication::translate("MainWindow", "Cam5", nullptr));
        ApyCam5->setText(QApplication::translate("MainWindow", "Cam4", nullptr));
        ApyToCam_8->setText(QApplication::translate("MainWindow", "\346\273\244\346\263\242\345\217\202\346\225\260", nullptr));
        ApyToCam_14->setText(QApplication::translate("MainWindow", "\351\253\230\346\226\257\346\273\244\346\263\242\345\217\202\346\225\260", nullptr));
        ApyToCam_11->setText(QApplication::translate("MainWindow", "\346\240\270\345\244\247\345\260\217", nullptr));
        ApyToCam_16->setText(QApplication::translate("MainWindow", "\344\270\255\345\200\274\346\273\244\346\263\242\345\217\202\346\225\260", nullptr));
        ApyToCam_17->setText(QApplication::translate("MainWindow", "\346\240\270\345\244\247\345\260\217", nullptr));
        ApyToCam_18->setText(QApplication::translate("MainWindow", "\346\260\264\345\271\263\350\247\222\345\272\246", nullptr));
        ApyToCam_19->setText(QApplication::translate("MainWindow", "\345\207\240\344\275\225\346\240\241\345\207\206\345\217\202\346\225\260", nullptr));
        ApyToCam_21->setText(QApplication::translate("MainWindow", "\347\253\226\347\233\264\350\247\222\345\272\246", nullptr));
        ApyToCam_20->setText(QApplication::translate("MainWindow", "\344\272\214\345\200\274\345\214\226\351\230\200\345\200\274", nullptr));
        ApyToCam_22->setText(QApplication::translate("MainWindow", "\351\230\200\345\200\274", nullptr));
        ApyToCam_24->setText(QApplication::translate("MainWindow", "\346\234\200\345\260\217\345\200\274", nullptr));
        ApyToCam_25->setText(QApplication::translate("MainWindow", "\345\242\236\347\233\212\350\214\203\345\233\264", nullptr));
        ApyToCam_13->setText(QApplication::translate("MainWindow", "\345\242\236\347\233\212\345\217\202\346\225\260", nullptr));
        ApyToCam_27->setText(QApplication::translate("MainWindow", "\351\242\221\346\254\241", nullptr));
        ApyToCam_28->setText(QApplication::translate("MainWindow", "\350\260\203\346\225\264\351\242\221\346\254\241", nullptr));
        ApyToCam_29->setText(QApplication::translate("MainWindow", "\346\234\200\345\244\247\345\200\274", nullptr));
        ApyToCam_30->setText(QApplication::translate("MainWindow", "\346\234\200\345\244\247\345\200\274", nullptr));
        ApyToCam_31->setText(QApplication::translate("MainWindow", "\351\230\200\345\200\274\350\214\203\345\233\264", nullptr));
        ApyToCam_32->setText(QApplication::translate("MainWindow", "\346\234\200\345\260\217\345\200\274", nullptr));
        SaveBtn->setText(QApplication::translate("MainWindow", "\344\277\235\345\255\230\345\217\202\346\225\260", nullptr));
        ApyToCam_12->setText(QApplication::translate("MainWindow", "RGB\345\216\237\345\233\276", nullptr));
        ApyToCam_15->setText(QApplication::translate("MainWindow", "\345\217\240\345\212\240\345\233\276\345\203\217", nullptr));
        menuCamSet->setTitle(QApplication::translate("MainWindow", "CamSet", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
