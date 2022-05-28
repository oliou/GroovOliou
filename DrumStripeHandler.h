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
#include "../JuceLibraryCode/JuceHeader.h"

#include "DrumStripeProcessor.h"
#include "CustomSamplerVoice.h"
#include "utils.h"


class DrumStripeProcessor;
class JuceDemoPluginAudioProcessorEditor;
class JuceDemoPluginAudioProcessor;

//==============================================================================
/** This is the editor component that our filter will display.
 */
class DrumStripeHandler:  public ButtonListener, //class DrumStripeHandler :public AudioProcessorEditor,
                        public SliderListener,
                        public Timer,
                        public ChangeListener,
                        public MouseListener,
                        public ComboBoxListener,
                        public KeyListener
{
public:
    //DrumStripeHandler(DrumStripeProcessor&);
    DrumStripeHandler(JuceDemoPluginAudioProcessor* ,
                     //DirectoryContentsList* ,
                     String ,
                     int,
                      int );
    
    ~DrumStripeHandler() ; //override;
    
    //==============================================================================
    void timerCallback() override;
    
    void setEditor(JuceDemoPluginAudioProcessorEditor* );
    void setGUIComponents( /* JuceDemoPluginAudioProcessorEditor* */ );
    void initializeSlider(/*JuceDemoPluginAudioProcessorEditor* editor,*/ Slider*, Label*, int, int, float, float);
    
    
    void buttonClicked (Button* )  override;
    
//    void buttonStateChanged(Button *) override;
    
    void updateDirectoryContentList();
    void changeListenerCallback(ChangeBroadcaster * ) override;
    void mouseDoubleClick (const MouseEvent& event ) override;
    void mouseDown (const MouseEvent& event ) override;
    void mouseEnter(const MouseEvent& event) override;
    void mouseExit(const MouseEvent& event) override;
    
    void comboBoxChanged(ComboBox*) override;
    
    void setSound(double);
    void loadNextSound();
    void loadPrevSound();
    void deleteSound();
    void updateSoundIndexFromSelection();
    
    void handleControlChange(int controllerNumber, int controllerValue );
    
    bool keyPressed( const KeyPress &  key,  Component * originatingComponent ) override;
    bool keyStateChanged( bool  isKeyDown,  Component * originatingComponent ) override;
    
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
    void sliderValueChanged (Slider*) override;
//    void sliderDragStarted  (Slider*) override;
//    void sliderDragEnded    (Slider*) override;
    
    int width = 100;
    int position = 0;
    int sliderHeight = 80;
    int note;
    
    bool canUpdateNote = false;
    
    bool soundLoaded = false;
    
    bool playStarted = false;
    
    
    int outputSelected = 1;
    ComboBox outputDropDown;
    
    int soundIndex = 0;
    int forcedSoundIndex = 0;
    int playMode = 1; //0=noteOnOff 1=oneshoot
    bool reverse = false;
    
    const String playButtonName="playButton";
    const String playButtonHelpMesg="Assigne Note: Ctrl-click then keyboard note.";
    
    const String sliderHelpMesg="Assign Controller: Ctrl-click then move controller. Double click to reset value.";

    const String volumeName="Gain";
    const String balanceName="Balance";
    const String pitchName="Pitch";
    const String attackName="Attack";
    const String decayName="Decay";
    
    const int volumeDefValue=1;
    const int balanceDefValue=0;
    const int pitchDefValue=1;
    const int attackDefValue=0;
    const int decayDefValue=0;
    
    float volume = volumeDefValue;
    float balance = balanceDefValue;
    float pitch = pitchDefValue;
    float attack = attackDefValue;
    float decay = decayDefValue;
    
    String samplesPath ;
    
    Label   sampleLabel,
            attackLabel,
            decayLabel,
            pitchLabel,
            volumeLabel,
            balanceLabel,
            outputLabel,
            noteLabel;
    
    Slider  attackSlider,
            decaySlider,
            pitchSlider,
            volumeSlider,
            balanceSlider;
    
    TextButton selectFileButton;
    TextButton selectFileFolderButton;
    TextButton  playButton;
    TextButton nextSoundButton;
    TextButton prevSoundButton;
    ToggleButton playModeButton;
    ToggleButton reverseButton;
    
    TimeSliceThread thread;
    DirectoryContentsList directoryContentsList;

    ListBoxModel *listBoxModel;
    
    AudioFormatManager formatManager;
    
    SamplerSound::Ptr soundObj;
    
    String selectionStr;
    
    bool hasThatSound();
    
    //DrumStripeProcessor *processor;
    
    void refreshView();
    
    JuceDemoPluginAudioProcessorEditor *mainEditor;
    JuceDemoPluginAudioProcessor *processor;
    
    void setLabel(String,bool);
    String getLabel();
        
    //DrumStripeHandler* getParameterFromSlider (const Slider*) const;
    
private:
    
    //DrumStripeHandler* getParameterFromSlider (const Slider*) const;
    
};


#endif /* defined(__Groovoliou2__DrumStripeHandler__) */

