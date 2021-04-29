#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    grabRGBThread1.init(img_rgb1,0);
    connect(&grabRGBThread1, SIGNAL(newImg(float)), this, SLOT(onImgRgb1(float)),Qt::QueuedConnection);
    grabRGBThread1.start();

    grabFIRThread1.init(img_fir1,1);
    connect(&grabFIRThread1, SIGNAL(newImg(float)), this, SLOT(onImgFir1(float)),Qt::QueuedConnection);
    grabFIRThread1.start();

    //grabRGBThread2.init(img_rgb2,1);
    //connect(&grabRGBThread2, SIGNAL(newImg(float)), this, SLOT(onImgRgb2(float)));
    //grabRGBThread2.start();

    detectObjectThread.init(img_rgb_full, rect);
    connect(&detectObjectThread, SIGNAL(newObject(float)), this, SLOT(onObject(float)),Qt::QueuedConnection);

    fuseImageThread.init(img_rgb_full, img_fir1, img_rgb_fused, calibration_fir_rgb);
    connect(&fuseImageThread, SIGNAL(newFusion(float)), this, SLOT(onImgFusion(float)),Qt::QueuedConnection);

    calibrateImageThread.init(img_rgb_full, img_fir1, calibration_fir_rgb);
    QTimer *timer = new QTimer(this);
    timer->start(1);
    connect(timer, SIGNAL(timeout()), this, SLOT(startApp()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete img_rgb1;
    delete img_rgb2;
    delete img_fir1;
    delete img_rgb_full;
    delete img_rgb_fir;
    delete img_rgb_fused;
    delete calibration_fir_rgb;

    delete rect;
}
