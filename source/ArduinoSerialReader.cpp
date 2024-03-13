#include "ArduinoSerialReader.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <iomanip>

ArduinoSerialReader::ArduinoSerialReader(const char* portName, speed_t baudRate, Processor& processor) 
    :   processorRef(processor), 
        serialPortFd_(-1), 
        portName_(portName), 
        baudRate_(baudRate), 
        stop_(false) 
{
    // Open serial port
    serialPortFd_ = open(portName_, O_RDWR | O_NOCTTY);
    if (serialPortFd_ < 0) 
    {
        perror("Error opening serial port");
        return;
    }

    // Configure serial port settings
    struct termios options;
    tcgetattr(serialPortFd_, &options);
    cfsetispeed(&options, baudRate_); // Set input baud rate
    options.c_cflag |= (CLOCAL | CREAD); // Enable receiver and set local mode
    tcsetattr(serialPortFd_, TCSANOW, &options);

    // Create thread for serial read operation
    readThread_ = std::thread(&ArduinoSerialReader::serialReadThread, this);
}

ArduinoSerialReader::~ArduinoSerialReader() 
{
    // Signal the thread to stop
    stop_ = true;

    // Wait for thread to finish
    readThread_.join();

    // Close serial port
    close(serialPortFd_);
}

void ArduinoSerialReader::serialReadThread() 
{
    // Buffer to store the received bytes (3 MIDI bytes + STOP_BYTE + null terminator)
    unsigned char buffer[4]; 
    size_t index = 0;

    while (!stop_) 
    {
        // Read from serial port
        unsigned char c;
        if (read(serialPortFd_, &c, 1) > 0) 
        {
            if (c == STOP_BYTE) 
            {
                // Process received data
                buffer[index] = '\0'; // Null-terminate the string
                processArduinoData(buffer);
                index = 0; // Reset index for the next packet
            } 
            else 
            {
                buffer[index++] = c;
                if (index >= sizeof(buffer)) 
                {
                    // Buffer overflow, discard the packet
                    index = 0;
                }
            }
        }
        usleep(1000);
    }
}

void ArduinoSerialReader::processArduinoData(unsigned char* data) 
{    
    // Check if the MIDI status byte indicates a Control Change message
    if (data[0] == MIDI_CC_STATUS_BYTE) {
        
        juce::MidiMessage message(data, 3);
        processorRef.handleMidiMessage(message);
    } 
    else 
    {
        DBG("Unexpected MIDI message received");
    }
}
