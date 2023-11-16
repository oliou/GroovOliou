/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"

#include "utils.h"

#include <string>


//==============================================================================
/**
*/
class GroovOliouAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        public juce::Slider::Listener,
                                        public juce::Timer
{
public:
    GroovOliouAudioProcessorEditor (GroovOliouAudioProcessor&);
    ~GroovOliouAudioProcessorEditor();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void sliderValueChanged (juce::Slider*) override;
    void sliderDragStarted  (juce::Slider*) override;
    void sliderDragEnded    (juce::Slider*) override;
    
    juce::MidiKeyboardComponent midiKeyboard;
    juce::Label versionLabel, infoLabel, gainLabel, delayLabel, balanceLabel, rmsR_Label,rmsL_Label ;
    juce::Slider gainSlider ,balanceSlider, delaySlider;
    
    void setDrumStripeEditor(); // set editor into drumstripes
    void refreshDrumStripeView();
    
    GroovOliouAudioProcessor& getProcessor() const
    {
        return static_cast<GroovOliouAudioProcessor&> (processor);
    }
    
    void setInfoMessage(juce::String,bool);
    juce::String getInfoMessage();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GroovOliouAudioProcessor& audioProcessor;
    
    juce::ScopedPointer<juce::ResizableCornerComponent> resizer;
    juce::ComponentBoundsConstrainer resizeLimits;

    juce::AudioPlayHead::CurrentPositionInfo lastDisplayedPosition;
    
    juce::TimeSliceThread thread;
    juce::DirectoryContentsList directoryContentsList;
    
    std::vector<DrumStripeHandler*> drumStripes;
    
    juce::AudioProcessorParameter* getParameterFromSlider (const juce::Slider*) const;
    
    void displayPositionInfo (const juce::AudioPlayHead::CurrentPositionInfo& pos);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GroovOliouAudioProcessorEditor)
};

