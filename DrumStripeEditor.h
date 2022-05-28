//
//  DrumStripe.h
//  Groovoliou2
//
//  Created by oliou on 15/07/2015.
//
//

#ifndef __Groovoliou2__DrumStripeEditor__
#define __Groovoliou2__DrumStripeEditor__

#include <stdio.h>
//#include "PluginEditor.h"
#include "../JuceLibraryCode/JuceHeader.h"

#include "DrumStripeProcessor.h"

class DrumStripeProcessor;
class JuceDemoPluginAudioProcessorEditor;

//==============================================================================
/** This is the editor component that our filter will display.
 */

class DrumStripeEditor  : public AudioProcessorEditor,
                          public SliderListener,
                          public Timer
{
public:
    DrumStripeEditor (AudioProcessor&, int);
    ~DrumStripeEditor();
    
    void initializeSlider(Slider*, Label*, int, int, float);
    
    //==============================================================================
    void timerCallback() override;
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged (Slider*) override;
    void sliderDragStarted  (Slider*) override;
    void sliderDragEnded    (Slider*) override;
    
   // MidiKeyboardComponent midiKeyboard;
    Label infoLabel, gainLabel, delayLabel,  rmsR_Label,rmsL_Label ;
    Slider gainSlider , delaySlider;
    
    String samplesPath;
    
    Label   sampleLabel,
    attackLabel,
    decayLabel,
    pitchLabel,
    volumeLabel,
    balanceLabel;
    
    Slider  attackSlider,
    decaySlider,
    pitchSlider,
    volumeSlider,
    balanceSlider;
    
    TextButton  playButton;
  //  TimeSliceThread thread;
  //  DirectoryContentsList directoryContentsList;
   
    
    //ListBoxModel *listBoxModel;

    int width = 100;
    int position = 0;
    int sliderHeight = 80;
    
private:
    
    
    ScopedPointer<ResizableCornerComponent> resizer;
    ComponentBoundsConstrainer resizeLimits;
    
    AudioPlayHead::CurrentPositionInfo lastDisplayedPosition;
    
//    TimeSliceThread thread;
//    DirectoryContentsList directoryContentsList;
    
    //DrumStripeEditor editor;
    
    std::vector<DrumStripeEditor*> drumStripes;
    
    //AudioProcessorGraph audioProcessorGraph;
    
    //AudioDeviceManager& fDeviceManager;
    
    //==============================================================================
    AudioProcessor& getProcessor() const
    {
        return static_cast<AudioProcessor&> (processor);
    }
    
    AudioProcessorParameter* getParameterFromSlider (const Slider*) const;
    
    void displayPositionInfo (const AudioPlayHead::CurrentPositionInfo& pos);
};

#endif /* defined(__Groovoliou2__DrumStripeEditor__) */

