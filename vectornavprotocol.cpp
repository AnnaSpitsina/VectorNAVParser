#include "vectornavprotocol.h"


VectorNavProtocol::VectorNavProtocol(QString portName, int baudRate, QObject *parent)
{
    m_port.setBaudRate(baudRate);
    m_port.setPortName(portName);
    m_port.open(QIODevice::ReadWrite);

    char cmd[20] = "$VNWRG,06,0*XX\r\n";
    m_port.write(cmd, 19);
    m_port.waitForBytesWritten();

    char cmd2[40] = "$VNWRG,75,2,20,01,0008*XX\r\n";
    m_port.write(cmd2, 39);
    m_port.waitForBytesWritten();

    connect(&m_port, &QSerialPort::readyRead, this, &VectorNavProtocol::readData);
}

unsigned short calculateCRC(unsigned char data[], unsigned int length) {
     unsigned int i;
     unsigned short crc = 0;
     for(i=0; i<length; i++){
         crc = (unsigned char)(crc >> 8) | (crc << 8);
         crc ^= data[i];
         crc ^= (unsigned char)(crc & 0xff) >> 4;
         crc ^= crc << 12;
         crc ^= (crc & 0x00ff) << 5;
     }
    return crc;
}

bool VectorNavProtocol::correctChecksum (QByteArray const &ba) {
    if (calculateCRC((uchar*)ba.data(), 8) == ba.data()[8]) {
        return true;
    }
    return false;
}

void VectorNavProtocol::readData() {
    m_buffer.append(m_port.readAll());
    parseBuffer();
}


void VectorNavProtocol::parseBuffer() {
    static int count;
    if ( m_buffer.size() <= 4 ) {
        return;
    }
    QByteArray header((char*) &(data.header),sizeof(Header));
    int index = m_buffer.indexOf(header);
    if (index == -1) {
        // Не найдено сообщение
        return;
    }
    if ( m_buffer.size() <= index + 17 ) {
        return;
    }

    if (correctChecksum(m_buffer.mid(index, 18))) {
        m_buffer.remove(0,index+17);
        return;
    }

    qDebug()<<++count;
    DataFromVectorNav* msg = reinterpret_cast<DataFromVectorNav*>(m_buffer.data()+index);

    qDebug() << "yaw: " << msg->yaw;
    qDebug() << "pitch: " << msg->pitch;
    qDebug() << "roll: " << msg->roll;
    qDebug() << m_buffer;
    m_buffer.remove(0, index+17);
    qDebug() << m_buffer;


//  emit newMessageDetected(*msg);

    //разбор m_buffer;
    //1. Если размер буфера меньше длины сообщения, то ничего не делаем,
    //выходим из метода по return и ждём, когда придёт достаточное количество байт

    //2. Если байт в буфере достаточное количество начинаем разбор
    // в QByteArray есть метод indexOf (QByteArray ba, int from=0) - который возвращает индекс,
    //где он первый раз встретил последовательность байт ba
    //QByteArray header((char*) &(data.header),sizeof(Header));// сделала QByteArray из структуры
    //int index = m_buffer.indexOf(header);//так можете получить индекс на первое нахождение заголовка в данных
    //возвращает -1, если ничего не нашёл

    //если найдено новое сообщение, то emit newMessageDetected()
}

