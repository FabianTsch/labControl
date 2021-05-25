#include"Atmega328p.h"
#include <chrono>
auto start = std::chrono::high_resolution_clock::now();
auto stop = std::chrono::high_resolution_clock::now();


EncoderControl::EncoderControl(QObject *parent) : QObject(parent)
{
    fd = wiringPiI2CSetup(DEVICE_ID);
    if (fd == -1)
        qDebug() << "Failed to init I2C communication.\n";
}


void EncoderControl::getResults()
{
    int data1[4] = {0};
    int data2[4] = {0};
    int data3[4] = {0};
    int buffer;
    float *ptr = (float*)&buffer;


    for(int i = 0; i < 4; i++)
        data1[i] = wiringPiI2CRead(fd);
    for(int i = 0; i < 4; i++)
        data2[i] = wiringPiI2CRead(fd);
    for(int i = 0; i < 4; i++)
        data3[i] = wiringPiI2CRead(fd);

    received_data_encoder = data1[0] | (data1[1] << 8) | (data1[2]<<16) | (data1[3] << 24);
    buffer = data2[0] | (data2[1] << 8) | (data2[2]<<16) | (data2[3] << 24);
    received_data_loadCell1 = *ptr;
    buffer = data3[0] | (data3[1] << 8) | (data3[2]<<16) | (data3[3] << 24);
    received_data_loadCell2 = *ptr;
    qDebug() << "Output: " << received_data_encoder;

    emit resultReady(received_data_encoder, received_data_loadCell1, received_data_loadCell2);

}

void EncoderControl::clear()
{
    int8_t resetBit = 1;
    wiringPiI2CWrite(fd, resetBit);
}
