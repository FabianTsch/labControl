#ifndef DAQHAT_H
#define DAQHAT_H

// Qt
#include <QObject>

// external headers
#include<daqhats/daqhats.h>
#include"daqhats_utils.h"


class Daqhat : public QObject
{
    Q_OBJECT

private:
    int result;
    bool error;
    uint8_t address;
    char display_header[512];
    char channel_string[512];
    uint8_t channel_mask;
    uint32_t samples_per_channel;
    int max_channel_array_length;
    int *channel_array;
    uint8_t num_channels;
    uint32_t internal_buffer_size_samples;
    uint32_t user_buffer_size;
    double *read_buf;
    int total_samples_read;
    int32_t read_request_size;
    double timeout;
    double scan_rate;
    double actual_scan_rate;
    uint32_t options;
    uint16_t read_status;
    uint32_t samples_read_per_channel;

public:
    explicit Daqhat(QObject *parent = nullptr);
    ~Daqhat();

public slots:
    void startPressed();
    void stopPressed();
    void getResults();

signals:
    void resultsReady(const QVector<double> &results, int, int);

};

#endif // DAQHAT_H
