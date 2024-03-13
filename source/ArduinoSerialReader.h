#include <thread>
#include <atomic>
#include <termios.h>

class ArduinoSerialReader
{
public:
    ArduinoSerialReader(const char* portName, speed_t baudRate);
    ~ArduinoSerialReader();

private:
    void serialReadThread();
    void processArduinoData(unsigned char* data);

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