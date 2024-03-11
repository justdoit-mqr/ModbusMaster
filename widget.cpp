#include "widget.h"
#include "ui_widget.h"
#include "modbusprotocol.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ModbusProtocol::getInstance();
}

Widget::~Widget()
{
    delete ui;
}
//读寄存器(可配合Qt Modbus Slave example进行测试验证，例程需要sudo启动，否则提示权限问题)
void Widget::on_readBtn_clicked()
{
    //线圈
    if(ui->typeComboBox->currentIndex() == 0)
    {
        qDebug()<<"read coils:"<<ModbusProtocol::getInstance()->readCoilsData(0,10);
    }
    //保持寄存器
    else if(ui->typeComboBox->currentIndex() == 1)
    {
        qDebug()<<"read holding:"<<ModbusProtocol::getInstance()->readHoldingData(0,10);
    }
}
//写寄存器(可配合Qt Modbus Slave example进行测试验证，例程需要sudo启动，否则提示权限问题)
void Widget::on_writeBtn_clicked()
{
    //线圈
    if(ui->typeComboBox->currentIndex() == 0)
    {
        QVector<quint16> values;
        values.append(0);
        values.append(1);
        values.append(0);
        values.append(0);
        values.append(1);
        values.append(0);
        qDebug()<<"write coils:"<<ModbusProtocol::getInstance()->writeCoilsData(0,values);
    }
    //保持寄存器
    else if(ui->typeComboBox->currentIndex() == 1)
    {
        QVector<quint16> values;
        values.append(5);
        values.append(10);
        values.append(24);
        values.append(13);
        values.append(15);
        values.append(1);
        qDebug()<<"write holding:"<<ModbusProtocol::getInstance()->writeHoldingData(0,values);
    }
}

