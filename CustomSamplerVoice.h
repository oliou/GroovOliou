//
//  CustomSamplerVoice.h
//  Groovoliou2
//
//  Created by oliou on 23/07/2015.
//
//

#ifndef __Groovoliou2__CustomSamplerVoice__
#define __Groovoliou2__CustomSamplerVoice__

#include <stdio.h>
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "DrumStripeHandler.h"

class JuceDemoPluginAudioProcessor;
class DrumStripeHandler;

//==============================================================================
/**
 A subclass of SynthesiserVoice that can play a SamplerSound.
 
 To use it, create a Synthesiser, add some SamplerVoice objects to it, then
 give it some SampledSound objects to play.
 
 @see SamplerSound, Synthesiser, SynthesiserVoice
 */
class JUCE_API  CustomSamplerVoice    : public SamplerVoice
{
public:
    //==============================================================================
    /** Creates a SamplerVoice. */
    CustomSamplerVoice(JuceDemoPluginAudioProcessor*);
    
    /** Destructor. */
    ~CustomSamplerVoice();
    
    //==============================================================================
    bool canPlaySound (SynthesiserSound*) override;
    
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound*, int pitchWheel) override;
    void stopNote (float velocity, bool allowTailOff) override;
    
    
    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;
    
    void renderNextBlock (AudioSampleBuffer&, int startSample, int numSamples) override;
    
    JuceDemoPluginAudioProcessor *processor;
    
    DrumStripeHandler *currDrumStripe=nullptr;
    
    double pitchRatio;
    float attackRatio=0.0;
    float decayRatio=0.0;
    double sourceSamplePosition;
    double sourceSampleRate;
    float lgain, rgain, attackReleaseLevel, attackDelta, releaseDelta;
    bool isInAttack, isInRelease;
    int playMode=0; //0=noteOnOff 1=oneshoot
    
private:
    //==============================================================================
    
    
    JUCE_LEAK_DETECTOR (CustomSamplerVoice)
};


#endif /* defined(__Groovoliou2__CustomSamplerVoice__) */