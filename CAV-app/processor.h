#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QObject>
#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>

using namespace cv;
using namespace std;

class Processor : public QObject
{
    Q_OBJECT
public:
    Processor();

    void setCount(int count);
    int count() const;
    int thickness() const;
    void setThickness(int thickness);
    int threshold() const;
    void setThreshold(int threshold);
    bool clicked() const;
    void setClicked(bool clicked);
    int getAlgo() const;
    void setAlgo(int algo);

public slots:
    void process(Mat* img);

signals:
    void processedImage(Mat*);

private:
    void getGradient(const Mat & img, Mat &gradX, Mat &gradY);
    void getSingularValue(const float a, const float b, const float d, Vec2f &singularVector, Vec2f &singularValue);
    void getTensor(const Mat & img, Mat &a, Mat &b, Mat &d, Mat &tensor);
    void drawTensor(const Mat & img);
    void createWhiteImage(int u, int v, Mat & white);
    Mat sketchingLinesWithTensor(const Mat & img);
    Mat sketchingLinesWithGradient(const Mat & img);
    Mat sketchingSplinesWithTensor(const Mat & img);
    Mat sketchingSplinesWithGradient(const Mat & img);

    int m_count = 5000;
    int m_thickness = 0;
    int m_threshold = 30;
    int m_length = 5;
    float m_percent = 0.1;
    int m_maxSize = 40;
    int m_algo = 0;

    Mat result;

    bool m_clicked = false;
};

#endif // PROCESSOR_H
