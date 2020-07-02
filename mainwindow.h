/*
* mainwindow.h
*
*  Created on  : Dec 10, 2017
*  Author      : Vinay Divakar
*  website     : www.deeplyemebedded.org
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/* Includes */
#include <QMainWindow>
#include<QTimer>
#include "ComSerial.h"
#include "vector"
#include "iostream"
#include "thread"
#include "fstream"
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
    void initGUI();

private slots:


    void on_bttnStartTest_clicked();

    void on_bttnStopTest_clicked();

    void on_bttnRPMApply_clicked();

    void on_bttnTimeApply_clicked();

public slots:
    void realtimePlot();
    void sendData();
    void receiveData();
    void saveDataInFile();


private:
    Ui::MainWindow *ui;
    unsigned long adc_data_g;
    QTimer timer_plot;
    ComSerial serial;
    std::thread rx_thread;
    QTimer timer_sender;
    double angle = 10;
    ofstream file;


};

#endif // MAINWINDOW_H
