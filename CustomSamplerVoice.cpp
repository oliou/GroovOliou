//
//  CustomSamplerVoice.cpp
//  Groovoliou2
//
//  Created by oliou on 23/07/2015.
//
//

#include "CustomSamplerVoice.h"

#include <math.h>

//==============================================================================
CustomSamplerVoice::CustomSamplerVoice(JuceDemoPluginAudioProcessor* processor)
: pitchRatio (0.0),
sourceSamplePosition (0.0),
lgain (0.0f), rgain (0.0f),
attackReleaseLevel (0), attackDelta (0), releaseDelta (0),
isInAttack (false), isInRelease (false),
processor(processor)
{
}

CustomSamplerVoice::~CustomSamplerVoice()
{
}

bool CustomSamplerVoice::canPlaySound (SynthesiserSound* sound)
{
    return dynamic_cast<const SamplerSound*> (sound) != nullptr;
}

void CustomSamplerVoice::startNote (const int midiNoteNumber,
                              const float velocity,
                              SynthesiserSound* s,
                              const int /*currentPitchWheelPosition*/)
{
    attackRatio=0;
    
    // update
    ModifierKeys mKey = ModifierKeys::getCurrentModifiers();
    
    if ( SamplerSound*  sound = dynamic_cast < SamplerSound*> (s))
    {
        pitchRatio = pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0)
        * sound->sourceSampleRate / getSampleRate();
        
        sourceSamplePosition = 0.0;
        lgain = velocity;
        rgain = velocity;
        
//        sourceSampleRate = sound->sourceSampleRate;
//        sound->attackSamples = roundToInt (1* sound->sourceSampleRate);
        
        isInAttack = (sound->attackSamples > 0);
        isInRelease = false;
        
        processor->info = processor->info + "attackSamples: " + (String) sound->attackSamples;
        
        if (isInAttack)
        {
            attackReleaseLevel = 0.0f;
            attackDelta = (float) (pitchRatio / sound->attackSamples);
        }
        else
        {
            attackReleaseLevel = 1.0f;
            attackDelta = 0.0f;
        }
        
        if (sound->releaseSamples > 0)
            releaseDelta = (float) (-pitchRatio / sound->releaseSamples);
        else
            releaseDelta = -1.0f;
    } else {
        jassertfalse; // this object can only play SamplerSounds!
    }
    
//    for (auto & drumStripe : processor->drumStripes) {
//        if(getCurrentlyPlayingNote() == drumStripe->note){
//            drumStripe->flashPlayButton(true);
//        }
//    }
}

void CustomSamplerVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        isInAttack = false;
        isInRelease = true;
    }
    else
    {
        clearCurrentNote();
    }
    
    float decay = 0;
    if(currDrumStripe != nullptr){
        decay = currDrumStripe->decay;
    }
    processor->info =  "stopnote: "+ (String) allowTailOff;
    
//    processor->info =" attackRatio:" + (String) attackRatio;
}

void CustomSamplerVoice::pitchWheelMoved (const int /*newValue*/)
{
}

void CustomSamplerVoice::controllerMoved (const int controllerNumber, const int newValue)
{
//    processor->info = "controllerMoved:" + (String) controllerNumber;
}

//==============================================================================
void CustomSamplerVoice::renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    //debugLog((String) __FUNCTION__  , true );
    
    int lIdx = 0;
    int rIdx = 1;
    float l_gain=lgain;
    float r_gain=rgain;
    bool reverse = false;
    float startPos = 0;
    
    for (auto & drumStripe : processor->drumStripes) {
        if(getCurrentlyPlayingNote() == drumStripe->note){
            
            reverse = drumStripe->reverse;
            startPos = drumStripe->attack;
            
            attackRatio+= (float) (drumStripe->attack/10);
//            1-(attackRatio+drumStripe->attack) * drumStripe->attack ;
            if(attackRatio>1){
                attackRatio = 1;
            }
            
//            drumStripe->volumeSlider.setValue(attackRatio);
            
//            processor->info =processor->info +"|"+ (String)numSamples + (String) log10(1+attackRatio);
            float temp = 1+attackRatio;
            
//            processor->info = (String) (1-drumStripe->attack+attackRatio);
            
            float gain=drumStripe->volume ;
            float balance=drumStripe->balance;
            
//            attackRatio = attackRatio + (1-drumStripe->attack);
            
            l_gain = gain*l_gain;
            r_gain = gain*r_gain;
            if(balance>0){
                l_gain= (1 - balance) *l_gain;
            }
            if(balance<0){
                r_gain= (1 - std::abs(balance)) *r_gain;
            }
            
//            processor->info = " #vel: " + (String)l_gain;
            
            pitchRatio = drumStripe->pitch;// * sound->sourceSampleRate / getSampleRate ();
            //attackDelta = drumStripe->attack;
            //releaseDelta = drumStripe->decay;
            attackReleaseLevel = drumStripe->attack;
            playMode = drumStripe->playMode;
            
            lIdx = drumStripe->outputSelected-1;
            rIdx = drumStripe->outputSelected;
            
            currDrumStripe = drumStripe;
            break;
        }
    }
    
    if (const SamplerSound* const playingSound = static_cast <SamplerSound*> (getCurrentlyPlayingSound().get()))
    {
        
        processor->info = " playingSound->length: " + (String) playingSound->length;
        
        //Reverse
        if(sourceSamplePosition == 0){
            if(!reverse){
                sourceSamplePosition = playingSound->length * startPos;
            }else{
                sourceSamplePosition = playingSound->length - (playingSound->length * startPos);
            }
        }
        
        const float* const inL = playingSound->data->getReadPointer (0);
        const float* const inR = playingSound->data->getNumChannels() > 1 ? playingSound->data->getReadPointer (1) : nullptr;
        
        float* outL = outputBuffer.getWritePointer (lIdx, startSample);
        float* outR = outputBuffer.getWritePointer (rIdx, startSample);
        
        while (--numSamples >= 0)
        {
            const int pos = (int) sourceSamplePosition;
            const float alpha = (float) (sourceSamplePosition - pos);
            const float invAlpha = 1.0f - alpha;
            
            // just using a very simple linear interpolation here..
            float l = (inL [pos] * invAlpha + inL [pos + 1] * alpha);
            float r = (inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha): l;
            
            l *= l_gain;
            r *= r_gain;
            
            if (isInAttack)
            {
                l *= attackReleaseLevel;
                r *= attackReleaseLevel;
                
                attackReleaseLevel += attackDelta;
                
                processor->info = " attackDelta: " + (String)attackReleaseLevel;
                
                if (attackReleaseLevel >= 1.0f)
                {
                    attackReleaseLevel = 1.0f;
                    isInAttack = false;
                }
            }
            else if (isInRelease and playMode == 0 ){
                l *= attackReleaseLevel;
                r *= attackReleaseLevel;
                
                attackReleaseLevel += releaseDelta;
                
                if (attackReleaseLevel <= 0.0f)
                {
                    stopNote (0.0f, false);
                    break;
                }
            }
            
            
            if (outR != nullptr)
            {
                *outL++ += l;
                *outR++ += r;
            }
            else
            {
                *outL++ += (l + r) * 0.5f;
            }
            
            if(!reverse){
                sourceSamplePosition += pitchRatio * playingSound->sourceSampleRate / getSampleRate ();
            }else{
                sourceSamplePosition -= pitchRatio * playingSound->sourceSampleRate / getSampleRate ();
            }
            
            if (sourceSamplePosition > playingSound->length or sourceSamplePosition < 0)
            {
                stopNote (0.0f, false);
                break;
            }
        }
        
//        for (auto & drumStripe : processor->drumStripes) {
//            if(getCurrentlyPlayingNote() == drumStripe->note){
//                drumStripe->playButton.setColour(TextButton::buttonColourId, Colours::blanchedalmond);
//            }
//        }
        
//        float gainL=1;
//        float gainR=1;
//        
//        for (auto & drumStripe : processor->drumStripes) {
//            if(getCurrentlyPlayingNote() == drumStripe->note){
//                //outputBuffer.applyGain(1, 0, outputBuffer.getNumSamples(), 0.5);
//                //buffer.applyGain (0, 0, buffer.getNumSamples(), gainL);
//                
//                float gain=drumStripe->volume;
//                gainL=gain;
//                gainR=gain;
//                float balance=drumStripe->balance;
//                
//                if(balance>0){
//                    gainL=gain*(1 - balance);
//                }
//                if(balance<0){
//                    gainR=gain*(1 + balance);
//                }
//            }
//        }
        
//        outputBuffer.applyGain (1, 0, outputBuffer.getNumSamples(), gainR);
//        outputBuffer.applyGain (0, 0, outputBuffer.getNumSamples(), gainL);
//
//        if(getCurrentlyPlayingNote() == 37){
//            outputBuffer.applyGain(0, 0, outputBuffer.getNumSamples(), 0.5);
//            //buffer.applyGain (0, 0, buffer.getNumSamples(), gainL);
//            
//        }
    }
}
