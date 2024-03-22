/*
    Written by Paul Compter, 10-03-2024
*/

#include <JuceHeader.h>
#include "AudioProcessor.h"
#include "ArduinoSerialReader.h"

int main (void)
{
    ScopedJuceInitialiser_GUI initialiser;
    juce::AudioDeviceManager deviceManager;
    Processor processor;
    juce::AudioProcessorPlayer player;

    player.setProcessor(&processor);

    deviceManager.addAudioCallback(&player);
    deviceManager.initialiseWithDefaultDevices(1,2);
    ArduinoSerialReader reader("/dev/cu.usbmodem14301", B9600, processor);

    for (; ;){};

    deviceManager.removeAudioCallback(&player);
    return 0;
}