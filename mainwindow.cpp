#include <QObject>
#include <QToolButton>

#include "mainwindow.h"

MainWindow::MainWindow(QMainWindow *MainWindow)
  : QObject()
  , Ui::MainWindow()
{
    setupUi(MainWindow);

    ship_1->loadFile(":/png/ship1.png");
    ship_2->loadFile(":/png/ship1.png");
    ship_3->loadFile(":/png/ship2.png");
    ship_4->loadFile(":/png/ship1.png");
    missile_3->loadFile(":/png/missile.png");
    video_1->setUrl(QUrl("qrc:/mp4/drive.mp4"));
    //video_2->setUrl(QUrl("qrc:/mp4/lorentz.mp4"));
    tides_1->loadFile(":/png/tides.png");
    dipole_1->loadFile(":/png/dipoles.png");
    video_3->setUrl(QUrl("qrc:/mp4/fits.mp4"));

    widget_6->setFramework("General Relativity");

    clock_1->startAt(100);
    clock_2->startAt(1);
    ship_1->startAt(100, 4.0);
    ship_2->startAt(1, 4.0, 0.0, -1, -1, Qt::darkBlue);
    missile_3->startAt(100, -300.0, 40.0, MainWindow->width(), 100);
    ship_4->startFor(1, MainWindow->width() / 2);
    video_1->play();

    connect(toolButton_1, SIGNAL(clicked()), this, SLOT(previousPage()));
    connect(toolButton_2, SIGNAL(clicked()), this, SLOT(nextPage()));
}

void MainWindow::previousPage()
{
    if (stackedWidget_1->currentIndex() > 0)
        stackedWidget_1->setCurrentIndex(stackedWidget_1->currentIndex() - 1);
}

void MainWindow::nextPage()
{
    if (stackedWidget_1->currentIndex() < stackedWidget_1->count() - 1)
        stackedWidget_1->setCurrentIndex(stackedWidget_1->currentIndex() + 1);
}
