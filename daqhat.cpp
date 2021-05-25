#include "daqhat.h"

Daqhat::Daqhat(QObject *parent) : QObject(parent)
{
    // Declaration of Variables
    result = RESULT_SUCCESS;
    address = 0;

    // Set the channel mask which is used by the library function
    // mcc118_a_in_scan_start to specify the channels to acquire.
    // The functions below, will parse the channel mask into a
    // character string for display purposes.
    channel_mask = {CHAN0 | CHAN1 | CHAN2 | CHAN3 | CHAN4 | CHAN5};
    samples_per_channel = 0;
    max_channel_array_length = mcc118_info()->NUM_AI_CHANNELS;
    channel_array = new int[max_channel_array_length];
    num_channels = 6;
    internal_buffer_size_samples = 0;
    user_buffer_size = 100000 * num_channels;
    read_buf = new double[user_buffer_size];
    total_samples_read = 0;
    read_request_size = READ_ALL_AVAILABLE;

    // When doing a continuous scan, the timeout value will be ignored in the
    // call to mcc118_a_in_scan_read because we will be requesting that all
    // available samples (up to the default buffer size) be returned.
    timeout = 5.0;
    scan_rate = 3000;
    actual_scan_rate = 0.0;
    mcc118_a_in_scan_actual_rate(num_channels, scan_rate, &actual_scan_rate);
    options = OPTS_CONTINUOUS;
    read_status = 0;
    samples_read_per_channel = 0;

    // Select an MCC118 HAT device to use.
    select_hat_device(HAT_ID_MCC_118, &address);

    // Open a connection to the device.
    result = mcc118_open(address);
    STOP_ON_ERROR(result);

}

Daqhat::~Daqhat(){
    delete [] channel_array;
    delete [] read_buf;
    mcc118_a_in_scan_stop(address);
    mcc118_a_in_scan_cleanup(address);
    mcc118_close(address);
}

void Daqhat::getResults(){
    QVector<double> results;

    // get Results from DAQ
    result = mcc118_a_in_scan_read(address, &read_status, read_request_size,
                timeout, read_buf, user_buffer_size, &samples_read_per_channel);
    STOP_ON_ERROR(result);
    total_samples_read += samples_read_per_channel;
    if(samples_read_per_channel > 0){

        for(unsigned int i = 0; i < (samples_read_per_channel*num_channels); i++){
            results.push_back(read_buf[i]);
        }
    }


    emit resultsReady(results, num_channels, samples_read_per_channel);
}

void Daqhat::startPressed(){
    result = mcc118_a_in_scan_start(address, channel_mask, samples_per_channel,
            scan_rate, options);
    STOP_ON_ERROR(result);
    result = mcc118_a_in_scan_buffer_size(address,
            &internal_buffer_size_samples);
    STOP_ON_ERROR(result);
}

void Daqhat::stopPressed(){
    result = mcc118_a_in_scan_stop(address);
    STOP_ON_ERROR(result);
    result = mcc118_a_in_scan_cleanup(address);
    STOP_ON_ERROR(result);
}
