#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initUi();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lineCountSpinBox_valueChanged(int arg1)
{
    emit lineCountChange(arg1);
}

void MainWindow::on_thicknessSpinBox_valueChanged(int arg1)
{
    emit thicknessChange(arg1);
}

void MainWindow::on_thresholdSpinBox_valueChanged(int arg1)
{
    emit thresholdChange(arg1);
}

void MainWindow::on_colorCheckBox_clicked(bool checked)
{
    emit colorClicked(checked);
}

void MainWindow::initUi()
{
    ui->comboBox->setEnabled(false);
    ui->colorCheckBox->setEnabled(false);
    ui->lineCountSpinBox->setEnabled(false);
    ui->thicknessSpinBox->setEnabled(false);
    ui->thresholdSpinBox->setEnabled(false);
    ui->runButton->setEnabled(false);
    ui->randomPointSpinBox->setEnabled(false);
    ui->pointOnEdgeSpinBox->setEnabled(false);
}

void MainWindow::displayRaw(Mat* img)
{
    Mat rgb;
    QPixmap p;
    cvtColor(*img, rgb, (-2*img->channels()+10));
    p.convertFromImage(QImage(rgb.data, rgb.cols, rgb.rows, QImage::Format_RGB888));
    ui->original->setPixmap(p);
    resize(img->cols, img->rows);
    ui->comboBox->setEnabled(true);
    ui->colorCheckBox->setEnabled(true);
    ui->lineCountSpinBox->setEnabled(true);
    ui->thicknessSpinBox->setEnabled(true);
    ui->thresholdSpinBox->setEnabled(true);
    ui->runButton->setEnabled(true);
}

void MainWindow::on_actionOuvrir_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,
           tr("Open Image"), "..", tr("Image Files (*.png *.jpg *.bmp)"));

    emit openImageTriggered(filename);
}

void MainWindow::on_actionQuitter_triggered()
{
    this->close();
}

void MainWindow::on_runButton_pressed()
{
    emit runButtonPressed();
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    emit algoChanged(index);
    if(index == 0 || index == 1 || index == 2 || index == 3 || index == 7)
    {
        ui->colorCheckBox->setEnabled(true);
        ui->lineCountSpinBox->setEnabled(true);
        ui->thicknessSpinBox->setEnabled(true);
        ui->thresholdSpinBox->setEnabled(true);
        ui->randomPointSpinBox->setEnabled(false);
        ui->pointOnEdgeSpinBox->setEnabled(false);
    }
    else
    {
        ui->colorCheckBox->setEnabled(false);
        ui->lineCountSpinBox->setEnabled(false);
        ui->thicknessSpinBox->setEnabled(false);
        ui->thresholdSpinBox->setEnabled(false);
        ui->randomPointSpinBox->setEnabled(true);
        ui->pointOnEdgeSpinBox->setEnabled(true);
    }
}

void MainWindow::on_pointOnEdgeSpinBox_valueChanged(int arg1)
{
    emit pointOnEdgeChange(arg1);
}

void MainWindow::on_randomPointSpinBox_valueChanged(int arg1)
{
    emit randomPointChange(arg1);
}
