#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QDebug>
#include "mainwindow.h"
#include "processor.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class controller : public QObject
{
    Q_OBJECT
public:
    controller(MainWindow *window, QObject *parent = 0);
    ~controller();
    void startApplication();

public slots:
    void openImage(QString filename);
    void colorClicked(bool clicked);
    void setLineCount(int count);
    void setThickness(int thickness);
    void setThreshold(int threshold);
    void setAlgo(int algo);
    void setPointOnEdge(int number);
    void setRandomPoint(int number);
    void startProcessing();
    void receiveProcessedImage(Mat* img);


signals :
    void rawImageToProcess(Mat* img);
    void rawImageToDisplay(Mat* img);
    void processedImageToDisplay(Mat* img);

private:
    MainWindow m_window;
    Processor m_processor;

    Mat m_rawImage;
    Mat m_imageProcessed;
};

#endif // CONTROLLER_H
