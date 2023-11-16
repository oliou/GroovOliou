//
//  DrumStripe.h
//  Groovoliou2
//
//  Created by oliou on 15/07/2015.
//
//

#ifndef __Groovoliou2__DrumStripeHandler__
#define __Groovoliou2__DrumStripeHandler__

#include <stdio.h>
#include <map>
//#include "PluginEditor.h"
#include <JuceHeader.h>
#include <string>
#include "DrumStripeHandler.h"


//#include "DrumStripeProcessor.h"
//#include "DrumSampler.h"
#include "utils.h"


//class DrumStripeProcessor;
class GroovOliouAudioProcessorEditor;
class GroovOliouAudioProcessor;

//==============================================================================
/** This is the editor component that our filter will display.
 */
class DrumStripeHandler:  public juce::Button::Listener, //class DrumStripeHandler :public AudioProcessorEditor,
                        public juce::Slider::Listener,
                        public juce::Timer,
                        public juce::ChangeListener,
                        public juce::MouseListener,
                        public juce::ComboBox::Listener,
                        public juce::KeyListener
{
public:
    //DrumStripeHandler(DrumStripeProcessor&);
    DrumStripeHandler(GroovOliouAudioProcessor* ,
                     //DirectoryContentsList* ,
                     juce::String ,
                     int,
                      int );
    
    ~DrumStripeHandler() ; //override;
    
    //==============================================================================
    void timerCallback() override;
    
    void setEditor(GroovOliouAudioProcessorEditor* );
    void setGUIComponents( /* JuceDemoPluginAudioProcessorEditor* */ );
    void initializeSlider(/*JuceDemoPluginAudioProcessorEditor* editor,*/ juce::Slider*, juce::Label*, int, int, float, float);
    
    
    void buttonClicked (juce::Button* )  override;
    
//    void buttonStateChanged(Button *) override;
    
    void updateDirectoryContentList();
    void changeListenerCallback(juce::ChangeBroadcaster * ) override;
    void mouseDoubleClick (const juce::MouseEvent& event ) override;
    void mouseDown (const juce::MouseEvent& event ) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    
    void comboBoxChanged(juce::ComboBox*) override;
    
    void setSound(double);
    void loadNextSound();
    void loadPrevSound();
    void deleteSound();
    void updateSoundIndexFromSelection();
    
    void handleControlChange(int controllerNumber, int controllerValue );
    
    bool keyPressed( const juce::KeyPress &  key,  juce::Component * originatingComponent ) override;
    bool keyStateChanged( bool  isKeyDown,  juce::Component * originatingComponent ) override;
    
    void flashPlayButton(bool forceFlash = false);
    
    void togglePlayModeButton(bool toggleState);
    void toggleReverseButton(bool toggleState);
    
    void updateNote(int newNote);
    
    void resetSlidersColour();
    
//    void paintButton (Graphics& g,
//                              bool isMouseOverButton,
//                      bool isButtonDown){} ;
    
  //  void timerCallback() override;
//    void paint (Graphics&) override;
 //   void resized() override;
    void sliderValueChanged (juce::Slider*) override;
//    void sliderDragStarted  (Slider*) override;
//    void sliderDragEnded    (Slider*) override;
    
    int width = 110;
    int position = 0;
    int sliderHeight = 93;
    int note;
    
    bool canUpdateNote = false;
    
    bool soundLoaded = false;
    
    bool playStarted = false;
    
    
    int outputSelected = 1;
    juce::ComboBox outputDropDown;
    
    int soundIndex = 0;
    int forcedSoundIndex = 0;
    int playMode = 1; //0=noteOnOff 1=oneshoot
    bool reverse = false;
    
    const juce::String playButtonName="playButton";
    const juce::String playButtonHelpMesg="Assigne Note: Ctrl-click then keyboard note. You can assign several samples to the same note.";
    
    const juce::String sliderHelpMesg="Assign Controller: Ctrl-click then move controller. Double click to reset value. ";
    const juce::String sliderReleaseHelpMesg= sliderHelpMesg + " Release affects the sound only in one-shot mode.";
    
    const juce::String playModeHelpMesg= "One-shot = Sample plays until defined release value | Note On/Off = Sample plays as long the key is pressed ";
    const juce::String reverseHelpMesg= "Reverse = Sample plays from the end. Use the \"Trim Start\" knob to adjust when playback starts.";

    
    const juce::String nextSoundHelpMesg="Click to load the next sample in the selected folder.";
    const juce::String prevSoundHelpMesg="Click to load the previous sample in the selected folder.";


    const juce::String volumeName="Gain";
    const juce::String balanceName="Balance";
    const juce::String pitchName="Pitch";
    const juce::String attackName="Trim Start";
    const juce::String releaseName="Release";
    const juce::String nextSoundName=" > ";
    const juce::String prevSoundName=" < ";
    const juce::String playModeName="PlayMode";
    const juce::String reverseName="Reverse";
    
    const int volumeDefValue=1;
    const int balanceDefValue=0;
    const int pitchDefValue=1;
    const int attackDefValue=0;
    const int releaseDefValue=1;
    
    float volume = volumeDefValue;
    float balance = balanceDefValue;
    float pitch = pitchDefValue;
    float attack = attackDefValue;
    float release = releaseDefValue;
    
    juce::String samplesPath ;
    
    juce::Label   sampleLabel,
            attackLabel,
            releaseLabel,
            pitchLabel,
            volumeLabel,
            balanceLabel,
            outputLabel,
            noteLabel;
    
    juce::Slider  attackSlider,
            releaseSlider,
            pitchSlider,
            volumeSlider,
            balanceSlider;
    
    juce::TextButton selectFileButton;
    juce::TextButton selectFileFolderButton;
    juce::TextButton  playButton;
    juce::TextButton nextSoundButton;
    juce::TextButton prevSoundButton;
    juce::ToggleButton playModeButton;
    juce::ToggleButton reverseButton;
    
    juce::TimeSliceThread thread;
    juce::DirectoryContentsList directoryContentsList;

    juce::ListBoxModel *listBoxModel;
    
    juce::AudioFormatManager formatManager;
    
    juce::SamplerSound::Ptr soundObj;
    
    juce::String selectionStr;
    
    bool hasThatSound();
    
    //DrumStripeProcessor *processor;
    
    void refreshView();
    
    GroovOliouAudioProcessorEditor *mainEditor;
    GroovOliouAudioProcessor *processor;
    
    void setLabel(juce::String,bool);
    juce::String getLabel();
        
    //DrumStripeHandler* getParameterFromSlider (const Slider*) const;
    
private:
    
    //DrumStripeHandler* getParameterFromSlider (const Slider*) const;
    
};


#endif /* defined(__Groovoliou2__DrumStripeHandler__) */

