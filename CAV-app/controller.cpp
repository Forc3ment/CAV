#include "controller.h"

using namespace cv;
using namespace std;

controller::controller(MainWindow* window, QObject *parent) :
    QObject(parent), m_window(window)
{
    qDebug() << m_processor.count();
    connect(&m_window,SIGNAL(colorClicked(bool)),this,SLOT(colorClicked(bool)));
    connect(&m_window,SIGNAL(lineCountChange(int)),this,SLOT(setLineCount(int)));
    connect(&m_window,SIGNAL(thresholdChange(int)),this,SLOT(setThreshold(int)));
    connect(&m_window,SIGNAL(thicknessChange(int)),this,SLOT(setThickness(int)));
    connect(&m_window,SIGNAL(openImageTriggered(QString)),this,SLOT(openImage(QString)));
    connect(&m_window,SIGNAL(algoChanged(int)),this,SLOT(setAlgo(int)));
    connect(&m_window,SIGNAL(pointOnEdgeChange(int)),this,SLOT(setPointOnEdge(int)));
    connect(&m_window,SIGNAL(randomPointChange(int)),this,SLOT(setRandomPoint(int)));

    connect(this, SIGNAL(rawImageToDisplay(Mat*)),&m_window,SLOT(displayRaw( Mat*)));
    //connect(this, SIGNAL(processedImageToDisplay(Mat*)),&m_window,SLOT(displayProcessed(Mat*)));
    connect(&m_window,SIGNAL(runButtonPressed()),this,SLOT(startProcessing()));
    connect(this, SIGNAL(rawImageToProcess(Mat*)),&m_processor,SLOT(process(Mat*)));
    connect(&m_processor,SIGNAL(processedImage(Mat*)),this,SLOT(receiveProcessedImage(Mat*)));
}

controller::~controller()
{

}

void controller::startApplication()
{
    m_window.show();
}

void controller::setThreshold(int threshold)
{
    qDebug() << threshold ;
    m_processor.setThreshold(threshold);
}

void controller::setThickness(int thickness)
{
    qDebug() << thickness ;
    m_processor.setThickness(thickness);
}

void controller::setLineCount(int count)
{
    qDebug() << count ;
    m_processor.setCount(count);
}

void controller::colorClicked(bool clicked)
{
    qDebug() << clicked ;
    m_processor.setClicked(clicked);
}

void controller::setAlgo(int algo)
{
    qDebug() << algo ;
    m_processor.setAlgo(algo);
}

void controller::setPointOnEdge(int number)
{
    qDebug() << number ;
    m_processor.setPointOnEdge(number);
}

void controller::setRandomPoint(int number)
{
    qDebug() << number ;
    m_processor.setRandomPoint(number);
}

void controller::receiveProcessedImage(Mat* img)
{
    emit processedImageToDisplay(img);
}

void controller::startProcessing()
{
    emit rawImageToProcess(&m_rawImage);
}

void controller::openImage(QString filename)
{
    if (!filename.isEmpty()){
        m_rawImage = imread(filename.toStdString(), CV_LOAD_IMAGE_COLOR);
        if(!m_rawImage.data ) { // Check for invalid input
            std::cout <<  "Could not open or find the image " << filename.toStdString() << std::endl ;
            waitKey(0); // Wait for a keystroke in the window
        }
        else
        {
            emit rawImageToDisplay(&m_rawImage);
        }

    }
}
