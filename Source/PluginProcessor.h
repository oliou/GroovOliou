/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "DrumStripeHandler.h"
#include "utils.h"

class DrumStripeHandler;

//==============================================================================
/**
*/
class GroovOliouAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    GroovOliouAudioProcessor();
    ~GroovOliouAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void reset() override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    const juce::String getInputChannelName (int channelIndex) const override;
    const juce::String getOutputChannelName (int channelIndex) const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override                                                   { return 1; }
    int getCurrentProgram() override                                                { return 0; }
    void setCurrentProgram (int /*index*/) override                                 {}
    const juce::String getProgramName (int /*index*/) override                       { return "Default"; }
    void changeProgramName (int /*index*/, const juce::String& /*newName*/) override  {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    bool checkPathExists(juce::String path, bool createFolder);
    
    void resetSlidersColour();
    
    // this is kept up to date with the midi messages that arrive, and the UI component
    // registers with it so it can represent the incoming messages
    juce::MidiKeyboardState keyboardState;

    // this keeps a copy of the last set of time info that was acquired during an audio
    // callback - the UI component will read this and display it.
    juce::AudioPlayHead::CurrentPositionInfo lastPosInfo;

    // these are used to persist the UI's size - the values are stored along with the
    // filter's other parameters, and the UI component will update them when it gets
    // resized.
    int lastUIWidth, lastUIHeight;
    
    std::map<int, juce::String> controlBindingMap;
    
    // Our parameters
    juce::AudioProcessorParameter* gain;
    juce::AudioProcessorParameter* balance;
    juce::AudioProcessorParameter* delay;
    
    juce::String setSample(juce::String samplePath,int note ) ;
    
    juce::String samplesPath="~/Library/Audio/Sounds/GroovOliou/";
    float  gainR, gainL;
    juce::String rmsR, rmsL, info;
    juce::AudioFormatManager formatManager;
    
    //void addVoices(AudioProcessorEditor*);
    
    // the synth!
    juce::Synthesiser synth;
    
    std::vector<DrumStripeHandler*> drumStripes;

    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GroovOliouAudioProcessor)
    
    juce::AudioSampleBuffer delayBuffer;
    int delayPosition;
};
