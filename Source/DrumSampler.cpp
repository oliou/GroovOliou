/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#include "DrumSampler.h"
#include "utils.h"
#include "PluginProcessor.h"


namespace juce
{

    DrumSamplerSound::DrumSamplerSound (const String& soundName,
                                AudioFormatReader& source,
                                const BigInteger& notes,
                                int midiNoteForNormalPitch,
                                double attackTimeSecs,
                                double releaseTimeSecs,
                                double maxSampleLengthSeconds)
        : name (soundName),
          sourceSampleRate (source.sampleRate),
          midiNotes (notes),
          midiRootNote (midiNoteForNormalPitch)
    {
        if (sourceSampleRate > 0 && source.lengthInSamples > 0)
        {
            length = jmin ((int) source.lengthInSamples,
                           (int) (maxSampleLengthSeconds * sourceSampleRate));

            data.reset (new AudioBuffer<float> (jmin (2, (int) source.numChannels), length + 4));

            source.read (data.get(), 0, length + 4, 0, true, true);

            params.attack  = static_cast<float> (attackTimeSecs);
            params.release = static_cast<float> (releaseTimeSecs);
        }
    }

    DrumSamplerSound::~DrumSamplerSound()
    {
    }

    bool DrumSamplerSound::appliesToNote (int midiNoteNumber)
    {
        return midiNotes[midiNoteNumber];
    }

    bool DrumSamplerSound::appliesToChannel (int /*midiChannel*/)
    {
        return true;
    }

    //==============================================================================
    DrumSamplerVoice::DrumSamplerVoice(GroovOliouAudioProcessor* p):
        processor(p)
//        pitchRatio (0.0),
//        sourceSamplePosition (0.0),
//        lgain (0.0f), rgain (0.0f),
//        attackReleaseLevel (0), attackDelta (0), releaseDelta (0),
//        isInAttack (false), isInRelease (false)
    {}

    DrumSamplerVoice::~DrumSamplerVoice() {}

    bool DrumSamplerVoice::canPlaySound (SynthesiserSound* sound)
    {
        return dynamic_cast<const DrumSamplerSound*> (sound) != nullptr;
    }

    void DrumSamplerVoice::startNote (int midiNoteNumber, float midiNoteVelocity, SynthesiserSound* s, int /*currentPitchWheelPosition*/)
    {
        debugLog( (juce::String) __PRETTY_FUNCTION__, true );
        
        // update
        if (auto* sound = dynamic_cast<const DrumSamplerSound*> (s))
        {
            pitchRatio = std::pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0)
                            * sound->sourceSampleRate / getSampleRate();
            
//            isInAttack = (sound->attackSamples > 0);
//            isInRelease = false;
            
            debugLog("midiNoteNumber: " + (juce::String) midiNoteNumber );
            debugLog("Velocity: " + (juce::String) velocity );
            
            sourceSamplePosition = 0.0;
            velocity = midiNoteVelocity;
//            lgain = velocity;
//            rgain = velocity;

            adsr.setSampleRate (sound->sourceSampleRate);
            adsr.setParameters (sound->params);
//            adsr.setParameters(juce::ADSR::Parameters(0.0f, 1.0f, 1.0f, 5.0f));

            debugLog( "ADSR START" );
            adsr.noteOn();
            debugLog( "ADSR END" );

        }
        else
        {
            debugLog( "sound NOT CASTED" );
            jassertfalse; // this object can only play SamplerSounds!
        }
    }

    void DrumSamplerVoice::stopNote (float /*velocity*/, bool allowTailOff)
    {
        if (allowTailOff)
        {
            adsr.noteOff();
        }
        else
        {
            clearCurrentNote();
            adsr.reset();
        }
    }

    void DrumSamplerVoice::pitchWheelMoved (int /*newValue*/) {}
    void DrumSamplerVoice::controllerMoved (int /*controllerNumber*/, int /*newValue*/) {}

    //==============================================================================
    void DrumSamplerVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
    {
//        debugLog((String) __FUNCTION__  , true );
        
        int lIdx = 0;
        int rIdx = 1;
//        float l_gain=lgain;
//        float r_gain=rgain;
        bool reverse = false;
        float startPos = 0;
        
        float lFinalGain = 0;
        float rFinalGain = 0;
        
        for (auto & drumStripe : processor->drumStripes) {
            if(getCurrentlyPlayingNote() == drumStripe->note){
//                debugLog("Position: "+ (String)  drumStripe->position );
                reverse = drumStripe->reverse;
                startPos = drumStripe->attack;
                                
                processor->info = drumStripe->samplesPath + drumStripe->sampleLabel.getText();
                
                //attackRatio+= (float) (drumStripe->attack/10);
                float attackRatio = drumStripe->attack;
    //            1-(attackRatio+drumStripe->attack) * drumStripe->attack ;
//                if(attackRatio>1){
//                    attackRatio = 1;
//                }
                float releaseRatio = drumStripe->release;
                if(drumStripe->playMode == 0){
                    float releaseRatio = 0;
                }
                
                adsr.setParameters(juce::ADSR::Parameters(
                                                          15,
                                                          adsr.getParameters().decay,
                                                          adsr.getParameters().sustain,
                                                          releaseRatio));
                
//                float temp = 1+attackRatio;
                float gain = drumStripe->volume ;
                float balance = drumStripe->balance;
                
                lFinalGain = gain*velocity;
                rFinalGain = gain*velocity;
                
                if(balance>0){
                    lFinalGain= (1 - balance) *lFinalGain;
                }
                if(balance<0){
                    rFinalGain= (1 - std::abs(balance)) *rFinalGain;
                }

                pitchRatio = drumStripe->pitch;// * sound->sourceSampleRate / getSampleRate ();
                
//                attackRatio = drumStripe->attack;
                int playMode = drumStripe->playMode;
                
                lIdx = drumStripe->outputSelected-1;
                rIdx = drumStripe->outputSelected;
                
                currDrumStripe = drumStripe;
                
                // Set Envelope
                break;
            
            }
        }
        
        if (auto* playingSound = static_cast<DrumSamplerSound*> (getCurrentlyPlayingSound().get()))
        {
//            processor->info = " playingSound->length: " + (String) playingSound->length;
            
            //Reverse
            if(sourceSamplePosition == 0){
                if(!reverse){
                    sourceSamplePosition = playingSound->length * startPos;
                }else{
                    sourceSamplePosition = playingSound->length - (playingSound->length * startPos);
                }
                debugLog((String) __FUNCTION__  , true );
                debugLog( "START"  );

                debugLog("sourceSamplePosition: "+ (String) sourceSamplePosition);
            
            }
            
            
            auto& data = *playingSound->data;
            const float* const inL = data.getReadPointer (0);
            const float* const inR = data.getNumChannels() > 1 ? data.getReadPointer (1) : nullptr;

            float* outL = outputBuffer.getWritePointer (0, startSample);
            float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;

            while (--numSamples >= 0)
            {
                auto pos = (int) sourceSamplePosition;
                auto alpha = (float) (sourceSamplePosition - pos);
                auto invAlpha = 1.0f - alpha;

                // just using a very simple linear interpolation here..
                float l = (inL[pos] * invAlpha + inL[pos + 1] * alpha);
                float r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[pos + 1] * alpha) : l;
//                float r = 0.05;

                
//                adsr.setParameters(juce::ADSR::Parameters(3.0f, 1.0f, 1.0f, 0.0f));
                auto envelopeValue = adsr.getNextSample();

                l *= lFinalGain * envelopeValue;
                r *= rFinalGain * envelopeValue;

//                debugLog("l: " + (String) l  + " | "  "r: " + (String) r  );
                
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
                    debugLog((String) __FUNCTION__  , true );
                    
                    debugLog("pitchRatio: "+ (String) pitchRatio);
                    debugLog("playingSound->sourceSampleRate: " + (String) playingSound->sourceSampleRate);
                    debugLog("pitchRatio: "+ (String) getSampleRate());
                    debugLog("playingSound->length: "+ (String) playingSound->length );
                    debugLog("sourceSamplePosition: "+ (String) sourceSamplePosition);
                    debugLog("numSamples: "+ (String) numSamples);
                    debugLog( "END"  );
                    
                    stopNote (0.0f, false);
                    break;
                }else{
                    
                }
            }
        }
    }

} // namespace juce
