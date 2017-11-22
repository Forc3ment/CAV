#include "mainwindow.h"
#include "controller.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Processor p;
    controller c(&w, &p);

    c.startApplication();

    return a.exec();
}
