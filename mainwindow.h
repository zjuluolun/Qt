#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QObject>
#include <QDebug>


#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

//#include <boost/thread/mutex.hpp>

#include<chrono>


//QReadWriteLock


//boost::mutex mutex;


class Sleeper : public QObject
{
    Q_OBJECT
public:
    static void sleep(float time)
    {
        std::chrono::steady_clock::time_point t1=std::chrono::steady_clock::now();
        while(1)
        {
            std::chrono::steady_clock::time_point t2=std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
            if(duration.count()>time) break;
        }
    }
};


class GrabRGBThread: public QThread
{
    Q_OBJECT
public:
    cv::Mat* img_;
    cv::VideoCapture capture;
    void init(cv::Mat * img, int id){
        img_ = img;
        capture.open(id);
//        capture.set(3, 1280);
//        capture.set(4, 720);
        capture.set(cv::CAP_PROP_FPS, 25);
        //capture.set(cv::CAP_PROP_EXPOSURE, 50);//曝光 50
    }
    virtual void run(){
        if(!capture.isOpened()){
            qDebug()<<"camera open failed";
            return;
        }
        while(true)
        {
            std::chrono::steady_clock::time_point t1=std::chrono::steady_clock::now();
            auto nanosec = t1.time_since_epoch();
            //qDebug()<<"读取视频失败";
            if(!capture.read(*img_))
            {
                qDebug()<<"读取视频失败1";
                continue;
            }


            std::chrono::steady_clock::time_point t2=std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
            //qDebug()<<"rgb:"<<duration.count();
            float timestamp=0;//nanosec.count()/(1000000.0);
            emit newImg(timestamp);
        }
    }
signals:
    void newImg(const float & timestamp) const;
};

class GrabFIRThread: public QThread
{
    Q_OBJECT
public:
    cv::Mat* img_;
    cv::VideoCapture capture;
    void init(cv::Mat * img, int id){
        img_ = img;
        capture.open(id);
//        capture.set(3, 1280);
//        capture.set(4, 720);
        capture.set(cv::CAP_PROP_FPS, 25);
        //capture.set(cv::CAP_PROP_EXPOSURE, 100);//曝光 50
    }
    virtual void run(){
        if(!capture.isOpened()){
            qDebug()<<"camera open failed";
            return;
        }
        while(true)
        {
            std::chrono::steady_clock::time_point t1=std::chrono::steady_clock::now();
            auto nanosec = t1.time_since_epoch();
            if(!capture.read(*img_))
            {
                //qDebug()<<"读取视频失败";
                continue;
            }

            std::chrono::steady_clock::time_point t2=std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
            //qDebug()<<"FIR:"<<duration.count();

            float timestamp=0;//nanosec.count()/(1000000.0);
            emit newImg(timestamp);
        }
    }
signals:
    void newImg(const float & timestamp) const;
};

class DetectObjectThread: public QThread
{
    Q_OBJECT
public:
    cv::Mat * img_;
    cv::Mat * rect_;
    void init(cv::Mat * img, cv::Mat* rect)
    {
        img_ = img;
        rect_ = rect;
    }
    virtual void run(){
        //mutex.lock();
        cv::Mat img = *img_;
        //mutex.unlock();
        //Sleeper().sleep(1000); //imitate the time cost operation;
        *rect_ = cv::Mat();// = obejectDetection(img_);

        std::chrono::steady_clock::time_point t1=std::chrono::steady_clock::now();
        auto nanosec = t1.time_since_epoch();
        float timestamp=0;//nanosec.count()/(1000000.0);
        emit newObject(timestamp);
    }
signals:
    void newObject(const float & timestamp) const;
};

class FuseImageThread: public QThread
{
    Q_OBJECT
public:
    cv::Mat * img_rgb_;
    cv::Mat * img_fir_;
    cv::Mat * img_fused_;
    cv::Mat * calibration_fir_rgb_;
    void init(cv::Mat * img_rgb, cv::Mat* img_fir, cv::Mat* img_fused, cv::Mat* calibration_fir_rgb)
    {
        img_rgb_ = img_rgb;
        img_fir_ = img_fir;
        img_fused_ = img_fused;
        calibration_fir_rgb_ = calibration_fir_rgb;
    }
    virtual void run(){
        //mutex.lock();
        std::chrono::steady_clock::time_point t0=std::chrono::steady_clock::now();
        cv::Mat img_rgb = *img_rgb_;
        cv::Mat img_fir = *img_fir_;
        cv::Mat img_fused = *img_fused_;
        //fusion()
        //mutex.unlock();
        //Sleeper().sleep(100); //imitate the time cost operation;
        cv::drawMatches( img_rgb, std::vector<cv::KeyPoint>(),
                         img_fir, std::vector<cv::KeyPoint>(),
                         std::vector<std::vector<cv::DMatch>>(), img_fused,
                         cv::Scalar::all(-1), cv::Scalar::all(-1),
                         std::vector<std::vector<char> >());;

        std::chrono::steady_clock::time_point t1=std::chrono::steady_clock::now();
//        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0);
//        qDebug()<<duration.count();

        auto nanosec = t1.time_since_epoch();
        float timestamp=0;//nanosec.count()/(1000000.0);
        *img_fused_ = img_fused;
        emit newFusion(timestamp);
    }
signals:
    void newFusion(const float & timestamp) const;
};

class CalibrateImageThread: public QThread
{
    Q_OBJECT
public:
    cv::Mat * img_rgb_;
    cv::Mat * img_fir_;
    cv::Mat * calibration_fir_rgb_;
    void init(cv::Mat * img_rgb, cv::Mat* img_fir, cv::Mat* calibration_fir_rgb)
    {
        img_rgb_ = img_rgb;
        img_fir_ = img_fir;
        calibration_fir_rgb_ = calibration_fir_rgb;
    }
    virtual void run(){
        //mutex.lock();
        cv::Mat calibration_fir_rgb = * calibration_fir_rgb_;
        Sleeper().sleep(4000); //imitate the time cost operation;
        //mutex.unlock();

        // do something to calibration_fir_rgb;

        * calibration_fir_rgb_ = calibration_fir_rgb;

        std::chrono::steady_clock::time_point t1=std::chrono::steady_clock::now();
        auto nanosec = t1.time_since_epoch();
        float timestamp=0;//nanosec.count()/(1000000.0);
        emit newCalibration(timestamp);
    }
signals:
    void newCalibration(const float & timestamp) const;
};






namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    cv::Mat* img_rgb1 = new cv::Mat();
    cv::Mat* img_rgb2 = new cv::Mat();
    cv::Mat* img_fir1 = new cv::Mat();

    cv::Mat* img_rgb_full = new cv::Mat();
    cv::Mat* img_rgb_fir = new cv::Mat();
    cv::Mat* img_rgb_fused = new cv::Mat();

    cv::Mat* calibration_fir_rgb = new cv::Mat();

    bool new_img_rgb1 = false;
    bool new_img_rgb2 = false;
    bool new_img_fir1 = false;

//    bool new_img_rgb_full = false;
//    bool img_rgb_fir = false;
    bool new_img_rgb_fused = false;

    float timestamp_img_rgb1 = 0;
    float timestamp_img_rgb2 = 0;
    float timestamp_img_fir1 = 0;
    float timestamp_img_obj = 0;

    bool new_rect=false;
    cv::Mat* rect=new cv::Mat();

    GrabRGBThread grabRGBThread1;
    GrabRGBThread grabRGBThread2;
    GrabFIRThread grabFIRThread1;
    DetectObjectThread detectObjectThread;
    FuseImageThread fuseImageThread;
    CalibrateImageThread calibrateImageThread;

    int count_frame=0;
    std::chrono::steady_clock::time_point count_duration_start=std::chrono::steady_clock::now();


private slots:
    //drivers
    void onImgRgb1(const float & timestamp)
    {
        new_img_rgb1=true;
        timestamp_img_rgb1=timestamp;
        ;//denoise
    }
    void onImgRgb2(const float& timestamp)
    {
        new_img_rgb2=true;
        timestamp_img_rgb2=timestamp;
        ;//denoise
    }
    void onImgFir1(const float& timestamp)
    {
        new_img_fir1=true;
        timestamp_img_fir1=timestamp;
        ;//denoise
    }

    void onObject(const float& timestamp)
    {
        new_rect=true;
        timestamp_img_obj = timestamp;
    }

    void onImgFusion(const float& timestamp)
    {
        new_img_rgb_fused=true;

    }

    void startApp()
    {
        showImg();
        //if(new_img_rgb1&&new_img_rgb2&&new_img_fir1&&(fabs(timestamp_img_rgb1-timestamp_img_rgb2)<0.01)&&(fabs(timestamp_img_rgb1-timestamp_img_fir1)<0.01)&&(fabs(timestamp_img_fir1-timestamp_img_rgb2)<0.01))
        if(new_img_rgb1&&new_img_fir1&&(fabs(timestamp_img_rgb1-timestamp_img_fir1)<30))
        {
            //mutex.lock();
            std::chrono::steady_clock::time_point t0=std::chrono::steady_clock::now();
            new_img_rgb1 = false;
            new_img_rgb2 = false;
            new_img_fir1 = false;
            cv::Mat img_rgb1_m = *img_rgb1;
            cv::Mat img_rgb2_m = *img_rgb2;
            cv::Mat img_fir1_m = *img_fir1;
            //mutex.unlock();


            registerRgb();
            fuseImageThread.run();//fuseRgbFir();

            //detectObjectThread.start(); //multiple start() only start once.
            //calibrateImageThread.start();

        }
        else
        {
            return;
        }
    }
    //tasks
    void registerRgb()
    {
//        cv::drawMatches( *img_rgb1, std::vector<cv::KeyPoint>(),
//                         *img_rgb2, std::vector<cv::KeyPoint>(),
//                         std::vector<std::vector<cv::DMatch>>(), *img_rgb_full,
//                         cv::Scalar::all(-1), cv::Scalar::all(-1),
//                         std::vector<std::vector<char> >());
        //img_rgb_full=img_rgb1+img_rgb2
        *img_rgb_full=*img_rgb1;
        //mySleep(5);
    }

    void fuseRgbFir()
    {
        cv::drawMatches( *img_rgb_full, std::vector<cv::KeyPoint>(),
                         *img_fir1, std::vector<cv::KeyPoint>(),
                         std::vector<std::vector<cv::DMatch>>(), *img_rgb_fused,
                         cv::Scalar::all(-1), cv::Scalar::all(-1),
                         std::vector<std::vector<char> >());;//img_rgb_fir=img_rgb_full+img_fir1;
        //Sleeper().sleep(200);
    }


    void detectionObject()
    {
        ;//img_rgb_fir
    }
    void showImg()
    {
        if(!new_img_rgb_fused) return;
        new_img_rgb_fused=false;
        if(new_rect)
        {
            drawRectImg();
            new_rect=false;
            plotImg();
            //showImg();
        }
        else
        {
            ;//showImg()
            plotImg();
        }
        ;//ig_rgb_fir
    }
    void drawRectImg()
    {
        ;//draw(img_rgb_fir, rect);
    }
    void plotImg()
    {
        if(this->img_rgb_fused->rows<=0) return;
        cv::imshow("img",*(this->img_rgb_fused));//
        cv::waitKey(1);

        auto t2 = std::chrono::steady_clock::now();
        auto count_time = std::chrono::duration_cast<std::chrono::microseconds>(t2 - this->count_duration_start);
        this->count_frame++;
        //qDebug()<<this->count_frame;
        if (count_frame==100)
        {
            qDebug() << "fps" << this->count_frame/(count_time.count()/1000000.0);
            this->count_duration_start=t2;
            this->count_frame=0;
        }
    }



};

#endif // MAINWINDOW_H
