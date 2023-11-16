/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "utils.h"



//==============================================================================
GroovOliouAudioProcessorEditor::GroovOliouAudioProcessorEditor (GroovOliouAudioProcessor& p)
    : AudioProcessorEditor (&p),
    midiKeyboard (p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
    audioProcessor (p),
    infoLabel (),
    gainLabel ("", "Throughput level:"),
    delayLabel ("", "Delay:"),
    balanceLabel ("", "Balance:"),
    rmsR_Label (),
    rmsL_Label (),
    gainSlider ("gain"),
    balanceSlider ("balance"),
    delaySlider ("delay"),
    thread("Directory Contents Scanning Thread"),
    directoryContentsList(nullptr, thread)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    debugLog( __PRETTY_FUNCTION__ , true );
    
    juce::String samplesPath ="~/Library/Audio/Plug-Ins/VST/GroovOliouRes/";
    
    thread.startThread();
    
    this->setDrumStripeEditor();
    
    // add some sliders..
    addAndMakeVisible (gainSlider);
    gainSlider.setSliderStyle (juce::Slider::Rotary);
    gainSlider.addListener (this);
    gainSlider.setRange (0.0, 1.0, 0.01);
    
    // add some sliders..
    addAndMakeVisible (balanceSlider);
    balanceSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    balanceSlider.addListener (this);
    balanceSlider.setRange(-1.0, 1.0, 0.01);
                        
    addAndMakeVisible (delaySlider);
    delaySlider.setSliderStyle (juce::Slider::Rotary);
    delaySlider.addListener (this);
    delaySlider.setRange (0.0, 1.0, 0.01);

    // add some labels for the sliders..
    gainLabel.attachToComponent (&gainSlider, false);
    gainLabel.setFont (juce::Font (11.0f));

    // add some labels for the sliders..
    balanceLabel.attachToComponent (&balanceSlider, false);
    balanceLabel.setFont (juce::Font (11.0f));
    
    delayLabel.attachToComponent (&delaySlider, false);
    delayLabel.setFont (juce::Font (11.0f));

    // add the midi keyboard component..
    addAndMakeVisible (midiKeyboard);

    // add a label that will display the current timecode and status..
    addAndMakeVisible (infoLabel);
    infoLabel.setColour (juce::Label::textColourId, juce::Colours::orange);
    
    // add a label that will display the current timecode and status..
    addAndMakeVisible (rmsR_Label);
    rmsR_Label.setColour (juce::Label::textColourId, juce::Colours::red);
    
    // add a label that will display the current timecode and status..
    addAndMakeVisible (rmsL_Label);
    rmsL_Label.setColour (juce::Label::textColourId, juce::Colours::green);

    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible (resizer = new juce::ResizableCornerComponent (this, &resizeLimits));
    resizeLimits.setSizeLimits (150, 150, 800, 600);


    // set our component's initial size to be the last one that was stored in the filter's settings
    //setSize (owner.lastUIWidth,
    //         owner.lastUIHeight);
    setSize (900, 800);
    startTimer (100);
    
}

GroovOliouAudioProcessorEditor::~GroovOliouAudioProcessorEditor()
{
}

//==============================================================================
void GroovOliouAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    
    g.setGradientFill (juce::ColourGradient (juce::Colours::darkgrey, 0, 0,
                                       juce::Colours::darkgrey, 0, (float) getHeight(), false));
  //  g.fillAll();
    
    
    juce::DrawableRectangle rect ;
    rect.setBounds(10,10,45,45);
    rect.setRectangle (juce::Rectangle<float> (0, 0, 50.0f, 50.0f));
    rect.setFill (juce::Colours::red);
    rect.setStrokeFill (juce::Colours::green);
    rect.setStrokeThickness (3.0f);
//
    addAndMakeVisible(rect);
    
    g.fillAll();
    
   // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

//    g.setColour (juce::Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Hello World! 1", getLocalBounds(), juce::Justification::centred, 1);
//    debugLog("teest 2");
//    debugLog( std::to_string( getWidth()) );
}

void GroovOliouAudioProcessorEditor::setDrumStripeEditor(){
    for (auto & drumStripe : getProcessor().drumStripes) {
        drumStripe->setEditor(this);
    }
}

void GroovOliouAudioProcessorEditor::refreshDrumStripeView(){
    for (auto & drumStripe : getProcessor().drumStripes) {
        drumStripe->refreshView();
    }
}

void GroovOliouAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    infoLabel.setBounds (10, 10, 900, 25);
    
    getProcessor().lastUIWidth = getWidth();
    getProcessor().lastUIHeight = getHeight();
    
    this->refreshDrumStripeView();
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void GroovOliouAudioProcessorEditor::timerCallback()
{
    GroovOliouAudioProcessor& ourProcessor = getProcessor();

    juce::AudioPlayHead::CurrentPositionInfo newPos (ourProcessor.lastPosInfo);

    if (lastDisplayedPosition != newPos)
        displayPositionInfo (newPos);

    gainSlider.setValue (ourProcessor.gain->getValue(), juce::dontSendNotification);
    delaySlider.setValue (ourProcessor.delay->getValue(), juce::dontSendNotification);
    balanceSlider.setValue (ourProcessor.balance->getValue(), juce::dontSendNotification);
    
    infoLabel.setText((juce::String) ourProcessor.info, juce::dontSendNotification);
    
    rmsR_Label.setText (rmsR_Label.getText() + (juce::String) ourProcessor.rmsR, juce::dontSendNotification);
    if(rmsR_Label.getText().length() > 100 ) {
        rmsR_Label.setText ("R: ", juce::dontSendNotification);
    }
    rmsL_Label.setText (rmsL_Label.getText() + (juce::String) ourProcessor.rmsL, juce::dontSendNotification);
    if(rmsL_Label.getText().length() > 100 ) {
        rmsL_Label.setText ("L: ", juce::dontSendNotification);
    }
}

void GroovOliouAudioProcessorEditor::setInfoMessage(juce::String message, bool append)
{
    if(!append){
        infoLabel.setText(message, juce::dontSendNotification);
    }else{
        infoLabel.setText(infoLabel.getText() + message, juce::dontSendNotification);
    }
}

juce::String GroovOliouAudioProcessorEditor::getInfoMessage()
{
    return infoLabel.getText();
}

// This is our Slider::Listener callback, when the user drags a slider.
void GroovOliouAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
//    if (juce::AudioProcessorParameter* param = getParameterFromSlider (slider))
//    {
//        // It's vital to use setValueNotifyingHost to change any parameters that are automatable
//        // by the host, rather than just modifying them directly, otherwise the host won't know
//        // that they've changed.
//        param->setValueNotifyingHost ((float) slider->getValue());
//    }
}

void GroovOliouAudioProcessorEditor::sliderDragStarted (juce::Slider* slider)
{
//    if (juce::AudioProcessorParameter* param = getParameterFromSlider (slider))
//    {
//        param->beginChangeGesture();
//    }
}

void GroovOliouAudioProcessorEditor::sliderDragEnded (juce::Slider* slider)
{
//    if (juce::AudioProcessorParameter* param = getParameterFromSlider (slider))
//    {
//        param->endChangeGesture();
//    }
}

//==============================================================================
// quick-and-dirty function to format a timecode string
static juce::String timeToTimecodeString (const double seconds)
{
    const double absSecs = std::abs (seconds);

    const int hours =  (int) (absSecs / (60.0 * 60.0));
    const int mins  = ((int) (absSecs / 60.0)) % 60;
    const int secs  = ((int) absSecs) % 60;

    juce::String s (seconds < 0 ? "-" : "");

    s << juce::String (hours).paddedLeft ('0', 2) << ":"
      << juce::String (mins) .paddedLeft ('0', 2) << ":"
      << juce::String (secs) .paddedLeft ('0', 2) << ":"
      << juce::String (juce::roundToInt (absSecs * 1000) % 1000).paddedLeft ('0', 3);

    return s;
}

// quick-and-dirty function to format a bars/beats string
static juce::String ppqToBarsBeatsString (double ppq, double /*lastBarPPQ*/, int numerator, int denominator)
{
    if (numerator == 0 || denominator == 0)
        return "1|1|0";

    const int ppqPerBar = (numerator * 4 / denominator);
    const double beats  = (fmod (ppq, ppqPerBar) / ppqPerBar) * numerator;

    const int bar    = ((int) ppq) / ppqPerBar + 1;
    const int beat   = ((int) beats) + 1;
    const int ticks  = ((int) (fmod (beats, 1.0) * 960.0 + 0.5));

    juce::String s;
    s << bar << '|' << beat << '|' << ticks;
    return s;
}

juce::AudioProcessorParameter* GroovOliouAudioProcessorEditor::getParameterFromSlider (const juce::Slider* slider) const
{
    if (slider == &gainSlider)
        return getProcessor().gain;
    
    if (slider == &balanceSlider)
        return getProcessor().balance;

    if (slider == &delaySlider)
        return getProcessor().delay;

    return nullptr;
}

// Updates the text in our position label.
void GroovOliouAudioProcessorEditor::displayPositionInfo (const juce::AudioPlayHead::CurrentPositionInfo& pos)
{
    lastDisplayedPosition = pos;
    juce::String displayText;
    displayText.preallocateBytes (128);

    displayText << juce::String (pos.bpm, 2) << " bpm, "
                << pos.timeSigNumerator << '/' << pos.timeSigDenominator
                << "  -  " << timeToTimecodeString (pos.timeInSeconds)
                << "  -  " << ppqToBarsBeatsString (pos.ppqPosition, pos.ppqPositionOfLastBarStart,
                                                    pos.timeSigNumerator, pos.timeSigDenominator);

    if (pos.isRecording)
        displayText << "  (recording)";
    else if (pos.isPlaying)
        displayText << "  (playing)";

//    infoLabel.setText ("[" + SystemStats::getJUCEVersion() + "]   " + displayText, dontSendNotification);
    //infoLabel.setText ("", dontSendNotification);
}
