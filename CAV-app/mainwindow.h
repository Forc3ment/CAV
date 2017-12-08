#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>

using namespace cv;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initUi();

private slots:
    void on_lineCountSpinBox_valueChanged(int arg1);
    void on_thicknessSpinBox_valueChanged(int arg1);
    void on_thresholdSpinBox_valueChanged(int arg1);
    void on_colorCheckBox_clicked(bool checked);
    void on_actionOuvrir_triggered();
    void on_actionQuitter_triggered();
    void on_runButton_pressed();
    void on_comboBox_currentIndexChanged(int index);
    void displayRaw(Mat* img);
    void on_pointOnEdgeSpinBox_valueChanged(int arg1);
    void on_randomPointSpinBox_valueChanged(int arg1);

signals:
    void lengthChange(int length);
    void lineCountChange(int count);
    void markingChange(double marking);
    void maxSizeChange(int maxSize);
    void thicknessChange(int thickness);
    void thresholdChange(int threshold);
    void colorClicked(bool checked);
    void pointOnEdgeChange(int arg1);
    void randomPointChange(int arg1);
    void openImageTriggered(QString filename);
    void runButtonPressed();
    void algoChanged(int index);

private:
    Ui::MainWindow *ui;
    void displayImg(const Mat& img, const QLabel label);
};

#endif // MAINWINDOW_H
