/*
 *@author:  缪庆瑞
 *@date:    2024.03.05
 *@brief:   modbus主站(客户端)组件，负责发送读写请求
 *该组件基于Qt serialbus模块中的modbus相关类实现，兼容串口rtu和网络tcp,
 */
#ifndef MODBUSMASTER_H
#define MODBUSMASTER_H

#include <QObject>
#include <QModbusClient>
#include <QtSerialPort/QSerialPort>

class ModbusMaster : public QObject
{
    Q_OBJECT
public:
    explicit ModbusMaster(bool useTcp=false,QObject *parent = nullptr);
    ~ModbusMaster();

    //配置接口参数
    void configRtuParam(QString portName,
                        QSerialPort::BaudRate baudRate = QSerialPort::Baud19200,
                        QSerialPort::Parity parity = QSerialPort::EvenParity,
                        QSerialPort::DataBits dataBits = QSerialPort::Data8,
                        QSerialPort::StopBits stopBits = QSerialPort::OneStop);
    void configTcpParam(QString addr="127.0.0.1",int port=502);
    //设置客户端属性
    void setClientProperty(int responseTimeMs=1000,int retriesNumber=3);

    //连接设备
    bool connectModbusDevice(bool reconnect);
    //发送读数据请求(同步)
    QVector<quint16> readModbusData(const QModbusDataUnit::RegisterType &dataType,
                                    const int &startAddr,const quint16 &size,const int &serverAddr);
    //发送写数据请求(异步)
    bool writeModbusData(const QModbusDataUnit::RegisterType &dataType,const int &startAddr,
                         const QVector<quint16> &values,const int &serverAddr);

private:
    QModbusClient *modbusDevice = nullptr;

signals:

private slots:
    void errorOccurredSlot(QModbusDevice::Error error);
    void stateChangedSlot(QModbusDevice::State state);

};

#endif // MODBUSMASTER_H
