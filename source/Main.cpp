#include <juce_core/juce_core.h>
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
    ArduinoSerialReader reader("/dev/cu.usbmodem14301", B9600);
    
    printf("Press 'q' to quit:\n");
    while (getchar() != 'q')
    {

    }

    deviceManager.removeAudioCallback(&player);
    return 0;
}
