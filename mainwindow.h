#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>

#include "ui_mainwindow.h"

class MainWindow : public QObject, public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QMainWindow *MainWindow);

public slots:
    void previousPage();
    void nextPage();
};

#endif // MAINWINDOW_H
