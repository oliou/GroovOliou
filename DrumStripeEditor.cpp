//
//  DrumStripe.cpp
//  Groovoliou2
//
//  Created by oliou on 15/07/2015.
//
//

#include "DrumStripeEditor.h"
#include "PluginEditor.h"

DrumStripeEditor::DrumStripeEditor(AudioProcessor& owner,
//                                   //JuceDemoPluginAudioProcessorEditor* editor,
//                                   DirectoryContentsList* directoryContentsList,
//                                   String samplesPath,
                                   int position):
    AudioProcessorEditor(owner),
    //midiKeyboard (owner.keyboardState, MidiKeyboardComponent::horizontalKeyboard),
    //mainEditor(editor),
    //processor(),
    position(position),
    samplesPath(samplesPath),
    sampleLabel("", "sample"),
    attackLabel("", "Attack"),
    decayLabel("", "Decay"),
    pitchLabel("", "Pitch"),
    balanceLabel("", "Balance"),
    volumeLabel("", "Volume"),
    attackSlider("Attack"),
    decaySlider("Decay"),
    pitchSlider("Pitch"),
    balanceSlider("Balance"),
    volumeSlider("Volume"),
    playButton((String)(position+1))
   // thread("Directory Contents Scanning Thread"),
   // directoryContentsList(nullptr, thread)
{
    this->addAndMakeVisible(sampleLabel);
    this->initializeSlider(&attackSlider, &attackLabel, 0, 1, 0.01);
    this->initializeSlider(&decaySlider, &decayLabel, 0, 1, 0.01);
    this->initializeSlider(&pitchSlider, &pitchLabel, 0, 1, 0.01);
    this->initializeSlider(&balanceSlider, &balanceLabel, -1, 1, 0.01);
    this->initializeSlider(&volumeSlider, &volumeLabel, 0, 1, 0.01);
    this->addAndMakeVisible(playButton);
    
//    String test = directoryContentsList->getFile(0);
//    sampleLabel.setText(directoryContentsList->getFile(0).getFileName(),dontSendNotification);
//    
//    mainEditor->infoLabel.setText(directoryContentsList->getFile(0).getFileName(), dontSendNotification);
    
};

DrumStripeEditor::~DrumStripeEditor()
{
}

void DrumStripeEditor::resized(){
    int shift = width*position;
    int hStart = 140;
    sampleLabel.setBounds (20+shift, 30, 100, 40);
    attackSlider.setBounds (20+shift, hStart, 60, 60);
    decaySlider.setBounds (20+shift, hStart + sliderHeight , 60, 60);
    pitchSlider.setBounds (20+shift, hStart + sliderHeight*2 , 60, 60);
    balanceSlider.setBounds (20+shift, hStart + sliderHeight*3 , 60, 60);
    volumeSlider.setBounds (20+shift, hStart + sliderHeight*4 , 60, 60);
    playButton.setBounds (20+shift, hStart + sliderHeight*5 , 50, 50);
    
}


void DrumStripeEditor::initializeSlider(juce::Slider *slider,
                                        Label *label,
                                        int min,
                                        int max,
                                        float step)
{
    this->addAndMakeVisible(slider);
    slider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    //attackSlider.setRotaryParameters(Slider::RotaryHorizontalVerticalDrag, float_Pi, true)
    slider->setTextBoxStyle(Slider::TextBoxBelow, true, 40, 20 );
    slider->addListener(this);
    slider->setRange (min, max, step);
    label->attachToComponent (slider, false);
    label->setFont (Font (11.0f));
}


void DrumStripeEditor::timerCallback()
{
    
    
}

void DrumStripeEditor::paint (Graphics& g)
{
    
}


void DrumStripeEditor::sliderValueChanged (Slider* slider)
{
    
}

void DrumStripeEditor::sliderDragStarted (Slider* slider)
{
 
}

void DrumStripeEditor::sliderDragEnded (Slider* slider)
{
   
}

// Updates the text in our position label.
void DrumStripeEditor::displayPositionInfo (const AudioPlayHead::CurrentPositionInfo& pos)
{
    
}


