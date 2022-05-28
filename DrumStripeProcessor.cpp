//
//  DrumStripe.cpp
//  Groovoliou2
//
//  Created by oliou on 15/07/2015.
//
//

#include "DrumStripeEditor.h"
#include "DrumStripeProcessor.h"
#include "PluginEditor.h"

DrumStripeProcessor::DrumStripeProcessor()
    //delayBuffer (2, 12000)
{

}


DrumStripeProcessor::~DrumStripeProcessor()
{
}

void DrumStripeProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{

}

void DrumStripeProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    keyboardState.reset();
}

void DrumStripeProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
    
}

AudioProcessorEditor* DrumStripeProcessor::createEditor()
{
    return new DrumStripeEditor (*this);
}


void DrumStripeProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    info = "Fromt drumproc";
}

void DrumStripeProcessor::getStateInformation (MemoryBlock& destData)
{

}

void DrumStripeProcessor::setStateInformation (const void* data, int sizeInBytes)
{
     
}

const String DrumStripeProcessor::getInputChannelName (const int channelIndex) const
{
    return String (channelIndex + 1);
}

const String DrumStripeProcessor::getOutputChannelName (const int channelIndex) const
{
    return String (channelIndex + 1);
}

bool DrumStripeProcessor::isInputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool DrumStripeProcessor::isOutputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool DrumStripeProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool DrumStripeProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool DrumStripeProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double DrumStripeProcessor::getTailLengthSeconds() const
{
    return 0.0;
}
    