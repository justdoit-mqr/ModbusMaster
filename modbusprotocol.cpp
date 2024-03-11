/*
 *@author:  缪庆瑞
 *@date:    2024.03.09
 *@brief:   modbus通信协议处理
 */
#include "modbusprotocol.h"
#include <QDebug>
#include <QTime>

/*
 *@brief:  获取实例对象
 *@date:   2024.03.09
 *@return: ModbusProtocol*
 */
ModbusProtocol *ModbusProtocol::getInstance()
{
    static ModbusProtocol instance;//静态局部对象第一次调用该方法时才初始化且只实例化一次
    return &instance;
}
/*
 *@brief:  构造函数
 *@date:   2024.03.09
 */
ModbusProtocol::ModbusProtocol(QObject *parent) : QObject(parent)
{
    //初始化modbusMaster设备
    modbusMaster = new ModbusMaster(true,this);
    if(!modbusMaster->connectModbusDevice(false))
    {
        qDebug()<<"connectModbusDevice failed!";
    }
}
