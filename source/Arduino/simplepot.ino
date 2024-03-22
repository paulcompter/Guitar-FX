const int numPots = 3; // Number of pots

// Define MIDI message components

#define STOP_BYTE 0xFF
#define MIDI_CONTROL_CHANGE_STATUS_BYTE 0xB0
#define THRESHOLD 2
#define MIDI_MIN_VALUE 0
#define MIDI_MAX_VALUE 127
#define ANALOG_MIN_VALUE 0
#define ANALOG_MAX_VALUE 1027
#define BAUD_RATE 9600


// Define a struct to hold potentiometer data
struct Potentiometer 
{
    int pin; // Analog input pin
    int ccNumber; // MIDI CC number
    int previousValue; // Previous potentiometer value
};

// Define an array of Potentiometer structs
Potentiometer pots[numPots] = 
{
    {A0, 1, -1}, // Define the pin, MIDI CC number, and initial value for each pot
    {A1, 2, -1},
    {A2, 3, -1}
};

void setup() 
{
    Serial.begin(BAUD_RATE);
}

void loop() 
{
    // Send potentiometer data as separate MIDI messages
    for (int i = 0; i < numPots; i++) 
    {
        int potValue = analogRead(pots[i].pin);
        int mappedPotValue = map(potValue, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, MIDI_MIN_VALUE, MIDI_MAX_VALUE);

        if (abs(mappedPotValue - pots[i].previousValue) > THRESHOLD) 
        {
            // Construct MIDI Control Change message as an array of bytes
            unsigned char midiMessage[] = 
            {
              MIDI_CONTROL_CHANGE_STATUS_BYTE | (pots[i].ccNumber & 0x00), // Set MIDI channel        
              (unsigned char)pots[i].ccNumber & 0x7F, // MIDI CC number
              (unsigned char)mappedPotValue & 0x7F, // MIDI value
              STOP_BYTE // Stop byte to indicate the end of the message
            };
          // Send MIDI message
          Serial.write(midiMessage, sizeof(midiMessage));
          pots[i].previousValue = mappedPotValue;
        }
    }

    // Add a small delay to prevent flooding the serial port
    delay(10);
}