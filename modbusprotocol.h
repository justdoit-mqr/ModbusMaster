/*
 *@author:  缪庆瑞
 *@date:    2024.03.09
 *@brief:   modbus通信协议处理
 *负责管理modbus master设备的读写操作，对接应用modbus通信协议，完成通信协议的封装与解析。
 */
#ifndef MODBUSPROTOCOL_H
#define MODBUSPROTOCOL_H

#include <QObject>
#include "modbusmaster.h"

class ModbusProtocol : public QObject
{
    Q_OBJECT
public:
    //单例模式(懒汉式,静态局部对象,自动完成资源释放)
    static ModbusProtocol *getInstance();

    //从mainServer读写数据
    QVector<quint16> readCoilsData(const int &startAddr,const quint16 &size){
        return modbusMaster->readModbusData(QModbusDataUnit::Coils,startAddr,
                                            size,mainServer);//读线圈
    }
    QVector<quint16> readHoldingData(const int &startAddr,const quint16 &size){
        return modbusMaster->readModbusData(QModbusDataUnit::HoldingRegisters,
                                            startAddr,size,mainServer);//读保持寄存器
    }
    bool writeCoilsData(const int &startAddr,const QVector<quint16> &values){
        return modbusMaster->writeModbusData(QModbusDataUnit::Coils,startAddr,
                                             values,mainServer);//写线圈
    }
    bool writeHoldingData(const int &startAddr,const QVector<quint16> &values){
        return modbusMaster->writeModbusData(QModbusDataUnit::HoldingRegisters,startAddr,
                                             values,mainServer);//写保持寄存器
    }


private:
    explicit ModbusProtocol(QObject *parent = 0);
    ModbusProtocol(const ModbusProtocol &);//拷贝构造私有化,不定义
    ModbusProtocol & operator=(const ModbusProtocol &);//拷贝赋值运算符私有化，不定义

    //定义modbus master设备操作对象
    ModbusMaster *modbusMaster = nullptr;
    const int mainServer = 8;//主机服务器

signals:

private slots:

};

#endif // MODBUSPROTOCOL_H
