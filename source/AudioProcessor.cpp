#include "AudioProcessor.h"

Processor::Processor() : juce::AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
                        treeState(*this, nullptr, juce::Identifier("Parameters"), createParameterLayout())
{
    auto& compressor = processorChain.template get<compressorIndex>();
    compressor.setAttack(40.0f);
    compressor.setRelease(20.0f);
    compressor.setRatio(3.0f);
    compressor.setThreshold(4.0f);

    auto& gain = processorChain.template get<gainIndex>();
    gain.setGainLinear(0.5f);

    auto& chorus = processorChain.template get<chorusIndex>();
    chorus.setCentreDelay(10.0f);
    chorus.setDepth(0.5f);
    chorus.setFeedback(0.0f);
    chorus.setMix(0.5f);
    chorus.setRate(20.0f);

    auto& reverb = processorChain.template get<reverbIndex>();
    reverbParams.roomSize = 1.0f;
    reverbParams.damping = 0.5f; 
    reverbParams.wetLevel = 0.33f;
    reverbParams.dryLevel = 0.4f;
    reverbParams.width = 1.0f;
    reverbParams.freezeMode = 0.0f; 
    reverb.setParameters(reverbParams);

    auto& masterGain = processorChain.template get<masterGainIndex>();
    masterGain.setGainLinear(0.5f);
}

Processor::~Processor()
{
    
}

const juce::String Processor::getName() const
{
    return "Guitar FX";
}

bool Processor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Processor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Processor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Processor::getTailLengthSeconds() const
{
    return 0.0;
}

int Processor::getNumPrograms()
{
    return 1;
}

int Processor::getCurrentProgram()
{
    return 0;
}

void Processor::setCurrentProgram (int index)
{
}

const juce::String Processor::getProgramName (int index)
{
    return {};
}

void Processor::changeProgramName (int index, const juce::String& newName)
{
}

void Processor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Processor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

bool Processor::hasEditor() const
{
    return false;
}

juce::AudioProcessorEditor* Processor::createEditor()
{
    return 0;
}

void Processor::getStateInformation (juce::MemoryBlock& destData)
{
}

void Processor::setStateInformation (const void* data, int sizeInBytes)
{
}

void Processor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    processorChain.reset();
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    processorChain.prepare(spec);
    
}

void Processor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    juce::dsp::AudioBlock<float> context (buffer);

    processorChain.process (juce::dsp::ProcessContextReplacing<float> (context));
}

juce::AudioProcessorValueTreeState::ParameterLayout Processor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    addParameters(layout);
    return layout;
}

void Processor::addParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // Compressor
    auto compressorAttack = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("COMPRESSORATTACK", 1), "Compressor Attack", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.5f);
    auto compressorRelease = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("COMPRESSORRELEASE", 1), "Compressor Release", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.5f);
    auto compressorRatio = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("COMPRESSORRATIO", 1), "Compressor Ratio", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.5f);
    auto compressorThreshold = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("COMPRESSORTHRESHOLD", 1), "Compressor Threshold", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.5f);
    auto groupCompressor = std::make_unique<juce::AudioProcessorParameterGroup>("compressor", "Compressor", "|",
                                                                      std::move (compressorAttack),
                                                                      std::move (compressorRelease),
                                                                      std::move (compressorRatio),
                                                                      std::move (compressorThreshold));
    // Pre Gain
    auto preGainLevel = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("PREGAIN", 1), "Pre Gain", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.5f);
    auto groupPreGain = std::make_unique<juce::AudioProcessorParameterGroup>("preGain", "PREGAIN", "|",
                                                                      std::move (preGainLevel));

    // Chorus
    auto chorusCentreDelay = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("CHORUSCENTREDELAY", 1), "Chorus Centre Delay", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.5f);
    auto chorusDepth = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("CHORUSDEPTH", 1), "Chorus Depth", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.5f);
    auto chorusFeedback = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("CHORUSFEEDBACK", 1), "Chorus Feedback", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.5f);
    auto chorusMix = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("CHORUSMIX", 1), "Chorus Mix", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.5f);
    auto chorusRate = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("CHORUSRATE", 1), "Chorus Rate", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.5f);
    auto groupChorus = std::make_unique<juce::AudioProcessorParameterGroup>("chorus", "CHORUS", "|",
                                                                      std::move (chorusCentreDelay),
                                                                      std::move (chorusDepth),
                                                                      std::move (chorusFeedback),
                                                                      std::move (chorusMix),
                                                                      std::move (chorusRate));
    // REVERB
    auto reverbRoomSize = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("REVERBROOMSIZE", 1), "Reverb Room Size",
                                                juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.0f);
    auto reverbDamping = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("REVERBDAMPING", 1), "Reverb Damping",
                                                juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.0f);
    auto reverbWetLevel = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("REVERBWETLEVEL", 1), "Reverb Wet Level",
                                                juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.0f);
    auto reverbDryLevel = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("REVERBDRYLEVEL", 1), "Reverb Dry Level",
                                                juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.0f);
    auto reverbWidth = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("REVERBWIDTH", 1), "Reverb Width",
                                                juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.0f);
    auto reverbFreezeMode = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("REVERBFREEZEMODE", 1), "Reverb Freeze Mode",
                                                juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.0f);
    auto groupReverb = std::make_unique<juce::AudioProcessorParameterGroup>("reverb", "REVERB", "|",
                                                                      std::move (reverbRoomSize),
                                                                      std::move (reverbDamping),
                                                                      std::move (reverbWetLevel),
                                                                      std::move (reverbDryLevel),
                                                                      std::move (reverbWidth),
                                                                      std::move (reverbFreezeMode));


    // Master Gain
    auto masterGain = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("MASTERGAIN", 1), "Master Gain",
                                                juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.5f);
    auto groupMaster = std::make_unique<juce::AudioProcessorParameterGroup>("master", "MASTER", "|",
                                                                      std::move (masterGain));     

    // Add all the groups to layout
    layout.add (std::move (groupCompressor),
                std::move (groupPreGain),
                std::move (groupChorus),
                std::move (groupReverb),
                std::move (groupMaster));

}

void Processor::handleMidiMessage(const MidiMessage& message)
{
    if (message.isController())
    {
        auto controllerNumber = message.getControllerNumber();
        auto controllerValue = message.getControllerValue();

        switch (controllerNumber)
        {
            case 1: // Example: Compressor Attack
                treeState.getParameter("COMPRESSORATTACK")->setValueNotifyingHost(controllerValue / 127.0f);
                DBG(controllerValue);
                break;
            // Add more cases for other MIDI CC numbers and parameters
        }
    }
}