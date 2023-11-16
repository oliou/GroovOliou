/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DrumSampler.h"


#include "utils.h"

#include <vector>
#include <string>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <fstream>

using namespace std;


juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();

class FloatParameter : public juce::AudioProcessorParameter
{
public:

    FloatParameter (float defaultParameterValue, const juce::String& paramName)
       : defaultValue (defaultParameterValue),
         value (defaultParameterValue),
         name (paramName)
    {
    }

    float getValue() const override
    {
        return value;
    }

    void setValue (float newValue) override
    {
        value = newValue;
    }

    float getDefaultValue() const override
    {
        return defaultValue;
    }

    juce::String getName (int maximumStringLength) const override
    {
        return name;
    }

    juce::String getLabel() const override
    {
        return juce::String();
    }

    float getValueForText (const juce::String& text) const override
    {
        return text.getFloatValue();
    }

private:
    float defaultValue, value;
    juce::String name;
};

const float defaultGain = 1.0f;
const float defaultBalance= 0.0f;
const float defaultDelay = 0.5f;


//==============================================================================
GroovOliouAudioProcessor::GroovOliouAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    : delayBuffer (2, 12000)
#endif
{
    debugLog(__PRETTY_FUNCTION__, true );
    
    // Set up our parameters. The base class will delete them for us.
    addParameter (gain  = new FloatParameter (defaultGain,  "Gain"));
    addParameter (delay = new FloatParameter (defaultDelay, "Delay"));
    addParameter (balance = new FloatParameter (defaultBalance, "Balance"));
    
    
    lastUIWidth = 400;
    lastUIHeight = 200;

    lastPosInfo.resetToDefault();
    delayPosition = 0;

    this->checkPathExists(samplesPath,true);
    
    // Initialise the synth...
    int startNote= 35;
    for (int i = 32; --i >= 0;){
//        synth.addVoice (new juce::SamplerVoice());
        synth.addVoice (new juce::DrumSamplerVoice(this));
        // These voices will play our custom sine-wave sounds..
    }
    for (int i = 0;++i < 9;){
        this->checkPathExists(samplesPath+"/"+(juce::String)i,true);
        drumStripes.push_back(new DrumStripeHandler(this,samplesPath,i,startNote+i));
    }
    
//    synth.addSound (new SineWaveSound());
    //    for (int i = 0; i < 16; ++i)
//    debugLog("TEST SYNTH");
//    synth.addVoice (new juce::SamplerVoice());
//    //    //samplesPath ="GroovOliouRes";
//       samplesPath ="~/Library/Audio/Sounds/GroovOliou";
//        juce::String sampledReady = this->setSample(samplesPath+"/1/000-Metrnome.WAV", 36);
//    //    sampledReady = this->setSample(samplesPath+"/2/HHOD0.WAV", 37);
//        synth.noteOn(1, 36, 1);
//        debugLog("after noteOn");
        //AlertWindow::showMessageBox(AlertWindow::InfoIcon, "test", "test");
       // DrumStripeProcessor  *proc = new DrumStripeProcessor();
    //    info = "Copy your samples under "+samplesPath +". Press Ctrl to assign control change.";
    
}

GroovOliouAudioProcessor::~GroovOliouAudioProcessor()
{
}

bool GroovOliouAudioProcessor::checkPathExists(juce::String path, bool createFolder)
{
    juce::String sMyFolderPath(path);
    juce::File myFolder( sMyFolderPath );
    bool isDirectory = myFolder.isDirectory();
    if ( myFolder.isDirectory() == false and createFolder )
    {
        return myFolder.createDirectory();
    }
    return isDirectory;
}

juce::String GroovOliouAudioProcessor::setSample(juce::String samplePath, int note)
{
    debugLog( __PRETTY_FUNCTION__, true );
    formatManager.registerBasicFormats();
    
    debugLog("samplePath: "+samplePath);
    
    juce::File audioFile(samplePath);
    
    // 音频文件中的数据“转交”给AudioFormatReader
    //reader = formatManager.createReaderFor (audioFile);
    std::unique_ptr<juce::AudioFormatReader> reader( formatManager.createReaderFor(audioFile));
    
    if (reader != nullptr)
    {
        juce::BigInteger notes;
        notes.setRange (note, 1, true);
        juce::DrumSamplerSound::Ptr sound = new juce::DrumSamplerSound (
                                                          samplePath,
                                                          *reader,
                                                          notes,
                                                          note,
                                                          0.0,
                                                          0.1,
                                                          60.0);
        synth.addSound (sound);
        
        debugLog("loaded");
        
        return "loaded";
    }else{
        debugLog("error");
        return "error";
    }
}

//==============================================================================
const juce::String GroovOliouAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GroovOliouAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GroovOliouAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GroovOliouAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GroovOliouAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//int GroovOliouAudioProcessor::getNumPrograms()
//{
//    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
//                // so this should be at least 1, even if you're not really implementing programs.
//}
//
//int GroovOliouAudioProcessor::getCurrentProgram()
//{
//    return 0;
//}
//
//void GroovOliouAudioProcessor::setCurrentProgram (int index)
//{
//}
//
//const juce::String GroovOliouAudioProcessor::getProgramName (int index)
//{
//    return {};
//}
//
//void GroovOliouAudioProcessor::changeProgramName (int index, const juce::String& newName)
//{
//}

//==============================================================================
void GroovOliouAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.setCurrentPlaybackSampleRate (sampleRate);
    keyboardState.reset();
    delayBuffer.clear();
}

void GroovOliouAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    keyboardState.reset();
}

void GroovOliouAudioProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
    delayBuffer.clear();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GroovOliouAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GroovOliouAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
    
    const int numSamples = buffer.getNumSamples();
    int channel, dp = 0;
    
    keyboardState.processNextMidiBuffer (midiMessages, 0, numSamples, true);
    
    juce::MidiBuffer processedMidi;
    int time;
    juce::MidiMessage m;
    
    gainR=gain->getValue();
    gainL=gain->getValue();
    
    if(balance->getValue()>0){
        //gainR=
        gainL=gain->getValue()*(1 - balance->getValue());
    }
    if(balance->getValue()<0){
        gainR=gain->getValue()*(1 + balance->getValue());
        //gainL=
    }
    
    for (juce::MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        if (m.isController())
        {
//            handleController (channel, m.getControllerNumber(), m.getControllerValue());
//            info = "controllerMoved2:" + (String) m.getControllerNumber();
            
            for (auto & drumStripe : drumStripes) {
                drumStripe->handleControlChange(m.getControllerNumber(), m.getControllerValue());
            }
            
        }
        else if (m.isProgramChange())
        {
//            handleProgramChange (channel, m.getProgramChangeNumber());
        }else if (m.isNoteOn()){
            for (auto & drumStripe : drumStripes) {
                drumStripe->updateNote(m.getNoteNumber());
            }
        }
    }
    
    synth.renderNextBlock (buffer, midiMessages, 0, numSamples);
}

//==============================================================================
bool GroovOliouAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GroovOliouAudioProcessor::createEditor()
{
    return new GroovOliouAudioProcessorEditor (*this);
}

//==============================================================================
void GroovOliouAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    debugLog((juce::String) __PRETTY_FUNCTION__, true );
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:

    // Create an outer XML element..
    juce::XmlElement xml ("MYPLUGINSETTINGS");

    // add some attributes to it..
    xml.setAttribute ("uiWidth", lastUIWidth);
    xml.setAttribute ("uiHeight", lastUIHeight);
    xml.setAttribute ("gain", gain->getValue());
    xml.setAttribute ("delay", delay->getValue());
    
    
    for (auto & drumStripe : drumStripes) {
        juce::String objectName = "drumStripe" +(juce::String)drumStripe->position +"_";
        xml.setAttribute (objectName+"samplesPath", drumStripe->samplesPath);
        xml.setAttribute (objectName+"soundIndex", drumStripe->soundIndex);
        xml.setAttribute (objectName+"forcedSoundIndex", drumStripe->soundIndex);
        xml.setAttribute (objectName+"playMode", drumStripe->playMode);
        xml.setAttribute (objectName+"reverse", drumStripe->reverse);
        xml.setAttribute (objectName+"note", drumStripe->note);
        xml.setAttribute (objectName+"volume", drumStripe->volume);
        xml.setAttribute (objectName+"balance", drumStripe->balance);
        xml.setAttribute (objectName+"pitch", drumStripe->pitch);
        xml.setAttribute (objectName+"attack", drumStripe->attack);
        xml.setAttribute (objectName+"release", drumStripe->release);
        xml.setAttribute (objectName+"outputSelected", drumStripe->outputSelected);
    }
    
    juce::String controlBindingStr;
    for (auto it=controlBindingMap.begin(); it!=controlBindingMap.end(); ++it){
        controlBindingStr+=(juce::String)it->first+";"+it->second+"|";
    }
    xml.setAttribute("controlBindingStr", controlBindingStr);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}

void GroovOliouAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    debugLog((juce::String) __PRETTY_FUNCTION__, true );
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("MYPLUGINSETTINGS"))
        {
            // ok, now pull out our parameters..
            lastUIWidth  = xmlState->getIntAttribute ("uiWidth", lastUIWidth);
            lastUIHeight = xmlState->getIntAttribute ("uiHeight", lastUIHeight);

            gain->setValue (xmlState->getDoubleAttribute ("gain", gain->getValue()));
            delay->setValue (xmlState->getDoubleAttribute ("delay", delay->getValue()));
            
            for (auto & drumStripe : drumStripes) {
                juce::String objectName = "drumStripe" +(juce::String)drumStripe->position +"_";
                //info="load "+ xmlState->getStringAttribute(objectName+"samplesPath",drumStripe->samplesPath);
                drumStripe->samplesPath = xmlState->getStringAttribute(objectName+"samplesPath",drumStripe->samplesPath);
                drumStripe->soundIndex = xmlState->getIntAttribute(objectName+"soundIndex",drumStripe->soundIndex);
                drumStripe->forcedSoundIndex = xmlState->getIntAttribute(objectName+"soundIndex",drumStripe->soundIndex);
                drumStripe->playMode = xmlState->getIntAttribute(objectName+"playMode",drumStripe->playMode);
                drumStripe->reverse = xmlState->getIntAttribute(objectName+"reverse",drumStripe->reverse);
                drumStripe->note = xmlState->getIntAttribute(objectName+"note",drumStripe->note);
                drumStripe->volume = xmlState->getDoubleAttribute(objectName+"volume",drumStripe->volume);
                drumStripe->balance = xmlState->getDoubleAttribute(objectName+"balance",drumStripe->balance);
                drumStripe->pitch = xmlState->getDoubleAttribute(objectName+"pitch",drumStripe->pitch);
                drumStripe->attack = xmlState->getDoubleAttribute(objectName+"attack",drumStripe->attack);
                drumStripe->release = xmlState->getDoubleAttribute(objectName+"release",drumStripe->release);
                drumStripe->outputSelected = xmlState->getIntAttribute(objectName+"outputSelected",drumStripe->outputSelected);
                //Refresh stripe with loaded params
                drumStripe->outputDropDown.setSelectedId(drumStripe->outputSelected);
                drumStripe->updateDirectoryContentList();
            }
            
            juce::String controlBindingStr = xmlState->getStringAttribute("controlBindingStr","");
            juce::StringArray cBindingList;
            cBindingList.addTokens(controlBindingStr, "|","");
            for (int i=0; i<cBindingList.size(); i++)
            {
                juce::StringArray bindLine;
                bindLine.addTokens(cBindingList[i], ";","");
                controlBindingMap[bindLine[0].getIntValue()]=bindLine[1];
            }
        }
    }
}

void GroovOliouAudioProcessor::resetSlidersColour(){
    debugLog( __PRETTY_FUNCTION__, true );
    for (auto & drumStripe : drumStripes) {
        drumStripe->resetSlidersColour();
    }
}

const juce::String GroovOliouAudioProcessor::getInputChannelName (const int channelIndex) const
{
    debugLog( __PRETTY_FUNCTION__, true );
    return juce::String (channelIndex + 1);
}

const juce::String GroovOliouAudioProcessor::getOutputChannelName (const int channelIndex) const
{
    debugLog( __PRETTY_FUNCTION__, true );
    return juce::String (channelIndex + 1);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    debugLog( __PRETTY_FUNCTION__, true );
    return new GroovOliouAudioProcessor();
}


