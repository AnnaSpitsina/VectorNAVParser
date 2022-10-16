#ifndef VECTORNAVPROTOCOL_H
#define VECTORNAVPROTOCOL_H

#include <QObject>
#include <QSerialPort>
#include <QDebug>
#include <QTimer>
;
//класс протокола
#pragma pack(push,1)
//тут сделала заглушку для заголовка послыки от VectorNav
struct Header {
    uint8_t sync = 0xFA;
    uint8_t group = 0x01;
    uint16_t group_1_fields = 0x08;
};

//сама структура, которая приходит от VectorNav
struct DataFromVectorNav {
    Header header;
    float yaw;
    float pitch;
    float roll;
    uint16_t crc;
};
#pragma pack(pop)

class VectorNavProtocol : public QObject
{
    Q_OBJECT
public:
    explicit VectorNavProtocol(QString portName, int baudRate = 115200, QObject *parent = 0);
    DataFromVectorNav data;//выходная структура

    bool correctChecksum (QByteArray const &ba);//это метод, который проверяет корректность чексуммы
signals:
    void newMessageDetected(DataFromVectorNav data);
public slots:
    void readData(); //слот, который будет вызываться в ответ на readyRead

protected:
    uint16_t calcCRC(QByteArray const &ba);
    void parseBuffer();
    QByteArray m_buffer;
    QSerialPort m_port; //объект COM-порта
    int baudRate = 115200; //бодрейт
};

#endif // VECTORNAVPROTOCOL_H
