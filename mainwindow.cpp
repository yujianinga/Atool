#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Serial_Init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Serial_Init()
{
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())  //查找可用串口
    {
        QSerialPort myserial;                   //定义串口类对象
        myserial.setPort(info);                 //设置串口端口
        if(myserial.open(QIODevice::ReadWrite)) //可读可写方式打开串口
        {
            ui->Select_Box->addItem(myserial.portName()); //添加串口号名字 并显示到comboBox
            myserial.close();
        }
    }
    ui->BaudRate_Box->setCurrentText("9600"); //设置默认波特率为9600
    ui->Operate_Button->setText("打开串口");
}

void MainWindow::on_Operate_Button_clicked()
{
    QString str=ui->Operate_Button->text();
    if("打开串口"==str)
    {
        if(ui->Select_Box->currentText()==NULL) QMessageBox::about(this,tr("提示"),tr("串口不可用！"));
        else {
            ui->BaudRate_Box->setEnabled(false);
            ui->Operate_Button->setText("关闭串口");
            ui->Select_Box->setEnabled(false);

            serial = new QSerialPort;
            serial->setPortName(ui->Select_Box->currentText());
            serial->open(QIODevice::ReadWrite);
            serial->setBaudRate(ui->BaudRate_Box->currentText().toUInt());
            serial->setDataBits(QSerialPort::Data8);
            serial->setStopBits(QSerialPort::OneStop);
            serial->setParity(QSerialPort::NoParity);
            serial->setFlowControl(QSerialPort::NoFlowControl);
            connect(serial,SIGNAL(readyRead()),this,SLOT(Receive_Data()));
        }
    }
    if("关闭串口"==str)
    {
        ui->Operate_Button->setText("打开串口");
        ui->BaudRate_Box->setEnabled(true);
        ui->Select_Box->setEnabled(true);

        serial->clear();
        serial->close();
        serial->deleteLater();
    }
}

void MainWindow::Receive_Data()
{
    QByteArray buf;

    buf=serial->readAll();
    ui->DisplayTXT_Brower->insertPlainText(buf+"\r\n");
    buf.clear();
}

void MainWindow::on_SerialSend_Button_clicked()
{
    QByteArray buf = ui->SerialInput0_Edit->toPlainText().toUtf8();
    serial->write(buf.data());
    buf.clear();
}

void MainWindow::on_SendClear_Button_clicked()
{
    ui->SerialInput0_Edit->clear();
}

