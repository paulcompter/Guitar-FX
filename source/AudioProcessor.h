/*
    Written by Paul Compter, 12-03-2024
*/


#pragma once

#include <JuceHeader.h>
#include "CustomDelay.h"

class Processor : public juce::AudioProcessor
{
public:
    Processor();
    ~Processor() override;
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void addParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void processParameters();
    void handleMidiMessage(const MidiMessage& message);
    
private:

    enum
    {
        compressorIndex,
        gainIndex,
        chorusIndex,
        delayIndex,
        reverbIndex,
        masterGainIndex
    };

    juce::AudioProcessorValueTreeState treeState;

    juce::dsp::ProcessorChain<juce::dsp::Compressor<float>, juce::dsp::Gain<float>,
                              juce::dsp::Chorus<float>, Delay<float>,
                              juce::dsp::Reverb, juce::dsp::Gain<float>> processorChain;
    
    juce::dsp::Reverb::Parameters reverbParams;
};