/*
* mainwindow.cpp
*
*  Created on  : Dec 10, 2017
*  Author      : Vinay Divakar
*  website     : www.deeplyemebedded.org
*/

#include<QDebug>
#include<QThread>
#include<QFont>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "unistd.h"
#include "math.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initGUI();
    file.open("data.csv");
    file << "rpm,current,voltage,temp,ref_t,Reference,duration";

    serial.initSerial();
    connect(&timer_sender, SIGNAL(timeout()),this,SLOT(sendData()));
    rx_thread = std::thread(&MainWindow::receiveData,this);

}

MainWindow::~MainWindow()
{
    delete ui;
    rx_thread.join();


}


void MainWindow::sendData()
{


    //    angle = angle + 0.5;
    //    double val = std::sin(180/3.14159) * angle;

    //    uint32_t duration;
    //    uint16_t rpm;
    //    uint16_t current;
    //    uint16_t voltage;
    //    uint16_t temp;
    //    time_t ref_t;
    //    int16_t reference;
    //    uint32_t sumatoria;
    //    uint8_t sumatoria;

    //    serial.txSendStruct.duration =  70+val;
    //    serial.txSendStruct.rpm =       10+val;
    //    serial.txSendStruct.current =   50+val;
    //    serial.txSendStruct.voltage =   60+val;
    //    serial.txSendStruct.temp =      30+val;
    //    serial.txSendStruct.ref_t =     40+val;
    //    serial.txSendStruct.reference = 80+val;
    //    serial.txSendStruct.code =      20+val;

    serial.transmitData();
}

void MainWindow::receiveData()
{

    bool receiveFlag = false;
    while(1)
    {

//        for testing
//        sendData();
//                for(uint8_t c: serial.txBuffer)
//                {
//                    receiveFlag = serial.receiveData(c);
//                }

//        serial.txBuffer.clear();

        receiveFlag = serial.receiveData();

        if(receiveFlag == true)
        {
            try {

//                for (auto c:serial.dataBuffer) {
//                    cout << c << " ";
//                    cout << endl;
//                }

                serial.extractData(serial.dataBuffer);

                ui->lblRPM->setText(QString::number(serial.rxStruct.rpm));
                ui->lblCurrent->setText(QString::number(serial.rxStruct.current));
                ui->lblVoltage->setText(QString::number(serial.rxStruct.voltage));
                ui->lblTemperature->setText(QString::number(serial.rxStruct.temp));
                ui->lblRefT->setText(QString::number(serial.rxStruct.ref_t));
                ui->lblReference->setText(QString::number(serial.rxStruct.reference));
                ui->lblDuration->setText(QString::number(serial.rxStruct.duration));


            }
            catch (std::exception ex) {
                std::cout << ex.what() <<endl;
            }



        }

        usleep(100000);
    }
}

void MainWindow::on_bttnStartTest_clicked()
{
    timer_plot.start(5);
    timer_sender.start(100);

    //from user to arduino via serial
    serial.txSendStruct.code = START;
    serial.txSendStruct.duration = ui->spinDuration->value();
    serial.txSendStruct.rpm = ui->spinRPM->value();

    sendData();

}

void MainWindow::on_bttnStopTest_clicked()
{
    timer_plot.stop();
    timer_sender.stop();
    serial.txSendStruct.code = STOP;
    sendData();

    file.flush();
    file.close();

}


void MainWindow::on_bttnRPMApply_clicked()
{
    serial.txSendStruct.code = RPM;
    serial.txSendStruct.rpm = static_cast<uint16_t>(ui->spinRPM->value());
    sendData();
}

void MainWindow::on_bttnTimeApply_clicked()
{
    serial.txSendStruct.code = TIME;
    sendData();
}

void MainWindow::initGUI()
{


    //current plot
    /* Add graph and set the curve line color to green */
    ui->CustomPlotCurrent->addGraph();
    ui->CustomPlotCurrent->graph(0)->setPen(QPen(Qt::red));
    ui->CustomPlotCurrent->graph(0)->setAntialiasedFill(false);

    /* Configure x-Axis as time in secs */
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%s");
    ui->CustomPlotCurrent->xAxis->setTicker(timeTicker);
    ui->CustomPlotCurrent->axisRect()->setupFullAxesBox();


    /* Configure x and y-Axis to display Labels */
    ui->CustomPlotCurrent->xAxis->setTickLabelFont(QFont(QFont().family(),8));
    ui->CustomPlotCurrent->yAxis->setTickLabelFont(QFont(QFont().family(),8));
    ui->CustomPlotCurrent->xAxis->setLabel("Time(s)");
    ui->CustomPlotCurrent->yAxis->setLabel("Current (A)");

    /* Make top and right axis visible, but without ticks and label */
    ui->CustomPlotCurrent->xAxis2->setVisible(true);
    ui->CustomPlotCurrent->yAxis->setVisible(true);
    ui->CustomPlotCurrent->xAxis2->setTicks(false);
    ui->CustomPlotCurrent->yAxis2->setTicks(false);
    ui->CustomPlotCurrent->xAxis2->setTickLabels(false);
    ui->CustomPlotCurrent->yAxis2->setTickLabels(false);

    // voltage plot
    ui->CustomPlotVoltage->addGraph();
    ui->CustomPlotVoltage->graph(0)->setPen(QPen(Qt::red));
    ui->CustomPlotVoltage->graph(0)->setAntialiasedFill(false);

    /* Configure x-Axis as time in secs */
    timeTicker->setTimeFormat("%s");
    ui->CustomPlotVoltage->xAxis->setTicker(timeTicker);
    ui->CustomPlotVoltage->axisRect()->setupFullAxesBox();


    /* Configure x and y-Axis to display Labels */
    ui->CustomPlotVoltage->xAxis->setTickLabelFont(QFont(QFont().family(),8));
    ui->CustomPlotVoltage->yAxis->setTickLabelFont(QFont(QFont().family(),8));
    ui->CustomPlotVoltage->xAxis->setLabel("Time(s)");
    ui->CustomPlotVoltage->yAxis->setLabel("Voltage(V)");

    /* Make top and right axis visible, but without ticks and label */
    ui->CustomPlotVoltage->xAxis2->setVisible(true);
    ui->CustomPlotVoltage->yAxis->setVisible(true);
    ui->CustomPlotVoltage->xAxis2->setTicks(false);
    ui->CustomPlotVoltage->yAxis2->setTicks(false);
    ui->CustomPlotVoltage->xAxis2->setTickLabels(false);
    ui->CustomPlotVoltage->yAxis2->setTickLabels(false);

    // temperature plot
    ui->CustomPlotTemperature->addGraph();
    ui->CustomPlotTemperature->graph(0)->setPen(QPen(Qt::red));
    ui->CustomPlotTemperature->graph(0)->setAntialiasedFill(false);

    /* Configure x-Axis as time in secs */
    timeTicker->setTimeFormat("%s");
    ui->CustomPlotTemperature->xAxis->setTicker(timeTicker);
    ui->CustomPlotTemperature->axisRect()->setupFullAxesBox();


    /* Configure x and y-Axis to display Labels */
    ui->CustomPlotTemperature->xAxis->setTickLabelFont(QFont(QFont().family(),8));
    ui->CustomPlotTemperature->yAxis->setTickLabelFont(QFont(QFont().family(),8));
    ui->CustomPlotTemperature->xAxis->setLabel("Time(s)");
    ui->CustomPlotTemperature->yAxis->setLabel("Temp (C)");

    /* Make top and right axis visible, but without ticks and label */
    ui->CustomPlotTemperature->xAxis2->setVisible(true);
    ui->CustomPlotTemperature->yAxis->setVisible(true);
    ui->CustomPlotTemperature->xAxis2->setTicks(false);
    ui->CustomPlotTemperature->yAxis2->setTicks(false);
    ui->CustomPlotTemperature->xAxis2->setTickLabels(false);
    ui->CustomPlotTemperature->yAxis2->setTickLabels(false);

    //RMS plot
    ui->CustomPlotRMS->addGraph();
    ui->CustomPlotRMS->graph(0)->setPen(QPen(Qt::red));
    ui->CustomPlotRMS->graph(0)->setAntialiasedFill(false);

    timeTicker->setTimeFormat("%s");
    ui->CustomPlotRMS->xAxis->setTicker(timeTicker);
    ui->CustomPlotRMS->axisRect()->setupFullAxesBox();
    ui->CustomPlotRMS->xAxis->setTickLabelFont(QFont(QFont().family(),8));
    ui->CustomPlotRMS->yAxis->setTickLabelFont(QFont(QFont().family(),8));
    ui->CustomPlotRMS->xAxis->setLabel("Time(s)");
    ui->CustomPlotRMS->yAxis->setLabel("RPM");
    ui->CustomPlotRMS->xAxis2->setVisible(true);
    ui->CustomPlotRMS->yAxis->setVisible(true);
    ui->CustomPlotRMS->xAxis2->setTicks(false);
    ui->CustomPlotRMS->yAxis2->setTicks(false);
    ui->CustomPlotRMS->xAxis2->setTickLabels(false);
    ui->CustomPlotRMS->yAxis2->setTickLabels(false);

    /* Set up and initialize the graph plotting timer */
    connect(&timer_plot, SIGNAL(timeout()),this,SLOT(realtimePlot()));


}

void MainWindow::realtimePlot()
{
    //    static QTime time(QTime::currentTime());
    //    double key = time.elapsed()/1000.0;
    //    static double lastPointKey = 0;

    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0;
    static double lastPointKey = 0;

    if(key - lastPointKey > 0.002)
    {
        ui->CustomPlotCurrent->graph(0)->addData(key, serial.rxStruct.current);
        ui->CustomPlotVoltage->graph(0)->addData(key, serial.rxStruct.voltage);
        ui->CustomPlotTemperature->graph(0)->addData(key, serial.rxStruct.temp);
        ui->CustomPlotRMS->graph(0)->addData(key, serial.rxStruct.rpm);

        lastPointKey = key;
        saveDataInFile();
    }

    /* make key axis range scroll right with the data at a constant range of 8. */
    ui->CustomPlotCurrent->graph(0)->rescaleValueAxis();
    ui->CustomPlotCurrent->xAxis->setRange(key, 60, Qt::AlignRight);
    ui->CustomPlotCurrent->replot();

    ui->CustomPlotVoltage->graph(0)->rescaleValueAxis();
    ui->CustomPlotVoltage->xAxis->setRange(key, 60, Qt::AlignRight);
    ui->CustomPlotVoltage->replot();

    ui->CustomPlotTemperature->graph(0)->rescaleValueAxis();
    ui->CustomPlotTemperature->xAxis->setRange(key, 60, Qt::AlignRight);
    ui->CustomPlotTemperature->replot();

    ui->CustomPlotRMS->graph(0)->rescaleValueAxis();
    ui->CustomPlotRMS->xAxis->setRange(key, 60, Qt::AlignRight);
    ui->CustomPlotRMS->replot();


}

void MainWindow::saveDataInFile()
{

    file << serial.rxStruct.rpm;
    file << ",";
    file << serial.rxStruct.current;
    file << ",";
    file << serial.rxStruct.voltage;
    file << ",";
    file << serial.rxStruct.temp;
    file << ",";
    file << serial.rxStruct.ref_t;
    file << ",";
    file << serial.rxStruct.reference;
    file << ",";
    file << serial.rxStruct.duration;
    file << "\n";

}
