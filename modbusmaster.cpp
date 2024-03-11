/*
 *@author:  缪庆瑞
 *@date:    2024.03.05
 *@brief:   modbus主站(客户端)组件，负责发送读写请求
 */
#include "modbusmaster.h"
#include <QModbusRtuSerialMaster>
#include <QModbusTcpClient>
#include <QEventLoop>
#include <QDebug>

/*
 *@brief:  构造函数
 *@date:   2024.03.05
 *@param:  useTcp:true=modbus-tcp  false=modbus-rtu(串口)
 */
ModbusMaster::ModbusMaster(bool useTcp, QObject *parent) :
    QObject(parent)
{
    if(useTcp)
    {
        modbusDevice = new QModbusTcpClient(this);
    }
    else
    {
        modbusDevice = new QModbusRtuSerialMaster(this);
    }

    connect(modbusDevice,&QModbusClient::errorOccurred,this,&ModbusMaster::errorOccurredSlot);
    //connect(modbusDevice,&QModbusClient::stateChanged,this,&ModbusMaster::stateChangedSlot);
}

ModbusMaster::~ModbusMaster()
{
    if(modbusDevice)
    {
        modbusDevice->disconnectDevice();
    }
}
/*
 *@brief:  配置rtu(串口)参数
 *QModbusDevice默认配置了一组串口常用参数(与函参默认参数一致)，可调用该接口修改
 *@date:   2024.03.05
 *@param:  portName:串口名
 *@param:  baudRate:波特率  parity:校验位  dataBits:数据位  stopBits:停止位
 */
void ModbusMaster::configRtuParam(QString portName, QSerialPort::BaudRate baudRate, QSerialPort::Parity parity,
                                  QSerialPort::DataBits dataBits, QSerialPort::StopBits stopBits)
{
    modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,portName);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,baudRate);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,parity);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,dataBits);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,stopBits);
}
/*
 *@brief:  配置tcp参数
 *QModbusDevice默认配置了一组tcp常用参数(与函参默认参数一致)，可调用该接口修改
 *@date:   2024.03.05
 *@param:  addr:地址  port:端口
 */
void ModbusMaster::configTcpParam(QString addr, int port)
{
    modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter,addr);
    modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter,port);
}
/*
 *@brief:  设置客户端属性
 *@date:   2024.03.05
 *@param:  responseTimeMs:响应超时时间 ms(发出请求后指定时间得不到响应，则超时报错)
 *@Param:  retriesNumber:重发次数(请求失败时尝试重发的次数)
 */
void ModbusMaster::setClientProperty(int responseTimeMs, int retriesNumber)
{
    modbusDevice->setTimeout(responseTimeMs);
    modbusDevice->setNumberOfRetries(retriesNumber);
}
/*
 *@brief:  连接modbus设备
 *注:这里的返回值只是表示连接处理是否被成功初始化，是否真正连接则需要根据state()状态判断
 *@date:   2024.03.06
 *@param:  reconnect:true=已连接的情况下断开重连
 *@return: bool:true=成功  false=失败
 */
bool ModbusMaster::connectModbusDevice(bool reconnect)
{
    if(modbusDevice->state() != QModbusDevice::ConnectedState)
    {
        return modbusDevice->connectDevice();
    }
    else
    {
        if(reconnect)
        {
            modbusDevice->disconnectDevice();
            return modbusDevice->connectDevice();
        }
        return true;
    }
}
/*
 *@brief:  读modbus数据(同步不阻塞界面)
 *注:modbus采用的是异步通信，QModbusReply默认也是通过finished信号异步处理，但这种异步方式在传递数据时不太友好(除了读取的数据外，
 *还需要记录读请求的数据类型、起始地址、服务器地址等)。所以这里通过QEventLoop实现一个同步但不阻塞界面的处理，方便对读取的数据进行处理。
 *@date:   2024.03.09
 *@param:  dataType:读的数据类型，分四类(线圈、离散输入(只读)、输入寄存器(只读)、保持寄存器)
 *@param:  startAddr:起始地址   size:数据块大小  serverAddr:服务器地址
 *@return: QVector<quint16>:数据块，出错返回空数组
 */
QVector<quint16> ModbusMaster::readModbusData(const QModbusDataUnit::RegisterType &dataType, const int &startAddr,
                                  const quint16 &size, const int &serverAddr)
{
    QVector<quint16> result_datas;
    //读数据时，没连接则尝试重连
    if(modbusDevice->state() != QModbusDevice::ConnectedState)
    {
        modbusDevice->connectDevice();
        return result_datas;
    }

    QModbusDataUnit dataUnit(dataType,startAddr,size);
    QModbusReply *reply = modbusDevice->sendReadRequest(dataUnit,serverAddr);
    if(reply)
    {
        if(!reply->isFinished())
        {
            QEventLoop eventLoop;
            connect(reply, &QModbusReply::finished,&eventLoop,&QEventLoop::quit);
            eventLoop.exec();
        }
        //出错提示
        if(reply->error() != QModbusDevice::NoError)
        {
            if(reply->error() == QModbusDevice::ProtocolError)
            {
                qDebug()<<QString("readModbusData error:%1(exception code = %2)").arg(reply->errorString())
                          .arg(reply->rawResult().exceptionCode(), -1, 16);
            }
            else
            {
                qDebug()<<QString("readModbusData error:%1").arg(reply->errorString());
            }
        }
        //处理应答
        else
        {
            const QModbusDataUnit unit = reply->result();
            if(unit.isValid())
            {
                result_datas = unit.values();
            }
        }
        reply->deleteLater();
    }

    return result_datas;
}
/*
 *@brief:  写modbus数据(发送写请求，不需要应答，该操作是异步的)
 *@date:   2024.03.09
 *@param:  dataType:写的数据类型，分四类(线圈、离散输入(只读)、输入寄存器(只读)、保持寄存器)
 *@param:  startAddr:起始地址   values:数据块  serverAddr:服务器地址
 *@return: bool:true=发送请求成功 false=请求失败
 */
bool ModbusMaster::writeModbusData(const QModbusDataUnit::RegisterType &dataType, const int &startAddr,
                                   const QVector<quint16> &values, const int &serverAddr)
{
    //写数据时，没连接则尝试重连
    if(modbusDevice->state() != QModbusDevice::ConnectedState)
    {
        modbusDevice->connectDevice();
        return false;
    }

    QModbusDataUnit dataUnit(dataType,startAddr,values.size());
    dataUnit.setValues(values);
    QModbusReply *reply = modbusDevice->sendWriteRequest(dataUnit,serverAddr);
    if(reply)
    {
        if(!reply->isFinished())
        {
            //出错提示
            connect(reply, &QModbusReply::finished, this, [this,reply](){
                if(reply->error() != QModbusDevice::NoError)
                {
                    if(reply->error() == QModbusDevice::ProtocolError)
                    {
                        qDebug()<<QString("writeModbusData error:%1(exception code = %2)").arg(reply->errorString())
                                  .arg(reply->rawResult().exceptionCode(), -1, 16);
                    }
                    else
                    {
                        qDebug()<<QString("writeModbusData error:%1").arg(reply->errorString());
                    }
                }
                reply->deleteLater();
            });
        }
        else
        {
            reply->deleteLater();
        }
        return true;
    }
    else
    {
        return false;
    }
}
/*
 *@brief:  发生错误信号处理槽
 *@date:   2024.03.05
 *@param:  error:错误码
 */
void ModbusMaster::errorOccurredSlot(QModbusDevice::Error error)
{
    if(error != QModbusDevice::NoError)
    {
        qDebug()<<"ModbusMaster error:"<<modbusDevice->errorString();
    }
}
/*
 *@brief:  连接状态改变信号处理槽
 *@date:   2024.03.05
 *@param:  state:状态
 */
void ModbusMaster::stateChangedSlot(QModbusDevice::State state)
{
    qDebug()<<"ModbusMaster state changed:"<<state;
}
