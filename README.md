This project is licensed under the GNU General Public License v3.0

# Guitar-FX
Juce Guitar FX Processor for Raspberry Pi with Arduino serial input

## Introduction

The purpose of this project is to run a headless RPI as a multi-FX processor using an Arduino UNO or its clones to
control the FX parameters.

Short Term Goals:
  -  To get it working ;)

Long Term Goals:
  - Control the Juce app through raw serial data
  - Add LCD support for the Arduino
  - Add the possibility of pre-programmed settings
  - Yadayada

## Features

Using the Juce framework makes it convenient to chain effects together, which is exactly what we need for a 
pedalboard. This should make it easy for users to add their own effects to the processor chain.
One of the big reasons for writing this software has been that we can set up the PI so that the SD card can
be set the Read-Only mode. This makes it convenient to not have to add a switch to turn off the device so 
users will not have to worry about SD card corruption.
The incorporation of directly using the Arduino's serial output without having to convert to MIDI seems like
a slight improvement.

## Installation

To use this project you will need
  - A Raspberry Pi
  - An Arduino
  - An Audio Interface

You're also going to need to clone the JUCE repository, and you'd want to install CMake to compile the source.
This was all written using VSCode which has the CMake extentions to make life easier.
The Arduino IDE is nice and easy for uploading the sketch to the board.

## Example usage

First clone the repository.
Add JUCE to the project directory.
Configure the CMakelists.txt to suit your needs.
Now you should be able to build using VSCode or the IDE of your choice.
I recommend to edit the AudioProcessor.cpp and header to accomodate things to your specific needs.

For the Arduino
Edit the Arduino/MidiController.ino file to add pots/pushbuttons etc.
Upload the sketch using the Arduino IDE.

## Contributing

Contributions to this project are welcome! Whether it's code contributions, bug reports, feature requests, or just ideas to make this project better, feel free to get involved. You can contribute by creating a new issue or submitting a pull request.

