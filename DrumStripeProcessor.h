//
//  DrumStripe.h
//  Groovoliou2
//
//  Created by oliou on 15/07/2015.
//
//

#ifndef __Groovoliou2__DrumStripeProcessor__
#define __Groovoliou2__DrumStripeProcessor__

#include <stdio.h>

#include "../JuceLibraryCode/JuceHeader.h"
#include "DrumStripeHandler.h"


//==============================================================================
/**
 As the name suggest, this class does the actual audio processing.
 */
class DrumStripeProcessor  : public AudioProcessor
{
    
public:
    DrumStripeProcessor();
    ~DrumStripeProcessor();
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages) override;
    void reset() override;
    
    bool hasEditor() const override                  { return true; }
    AudioProcessorEditor* createEditor() override;
    const String getName() const override            { return JucePlugin_Name; }
    
    const String getInputChannelName (int channelIndex) const override;
    const String getOutputChannelName (int channelIndex) const override;
    bool isInputChannelStereoPair (int index) const override;
    bool isOutputChannelStereoPair (int index) const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override                                               { return 1; }
    int getCurrentProgram() override                                            { return 0; }
    void setCurrentProgram (int /*index*/) override                             {}
    const String getProgramName (int /*index*/) override                        { return "Default"; }
    void changeProgramName (int /*index*/, const String& /*newName*/) override  {}
    
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
      
    //AudioSampleBuffer delayBuffer;
    MidiKeyboardState keyboardState;
    
    String info;
    
private:
    
    
};


#endif /* defined(__Groovoliou2__DrumStripeProcessor__) */

