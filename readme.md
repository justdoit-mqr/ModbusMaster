# ModbusMaster
这是一个基于Qt serialbus模块封装的modbus主站(客户端)组件。
## 功能概述:
ModbusMaster组件基于Qt serialbus模块中的modbus相关类实现，兼容串口rtu和网络tcp。
其中ModbusMaster类中主要封装QModbus*相关的类接口，实现modbus设备的参数配置与数据读写操作，而ModbusProtocol则面向具体应用提供更加便捷的接口完成读写操作，并以此类维护应用的modbus通信协议。
## 作者联系方式
**邮箱:justdoit_mqr@163.com**  
