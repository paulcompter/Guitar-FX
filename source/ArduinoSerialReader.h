/*
    Written by Paul Compter, 10-03-2024
*/

#pragma once

#include <thread>
#include <atomic>
#include <termios.h>
#include "AudioProcessor.h"

class ArduinoSerialReader
{
public:
    ArduinoSerialReader(const char* portName, speed_t baudRate, Processor& processor);
    ~ArduinoSerialReader();

private:
    Processor& processorRef;
    void serialReadThread();
    void processArduinoData(unsigned char* data);
    void prepare();

    unsigned char serialBuffer[4];

    int serialPortFd_;
    const char* portName_;
    speed_t baudRate_;
    std::thread readThread_;
    std::atomic<bool> stop_;

    // Byte definitions
    static constexpr unsigned char MIDI_CC_STATUS_BYTE = 0xB0U;
    static constexpr unsigned char MIDI_CC_NUMBER = 1;
    static constexpr unsigned char MIDI_VALUE = 2;
    static constexpr unsigned char STOP_BYTE = 0xFFU;
};