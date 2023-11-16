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

#include <JuceHeader.h>

#include <math.h>

#include "utils.h"
#include "PluginProcessor.h"


namespace juce
{

    //==============================================================================
    /**
        A subclass of SynthesiserSound that represents a sampled audio clip.

        This is a pretty basic sampler, and just attempts to load the whole audio stream
        into memory.

        To use it, create a Synthesiser, add some SamplerVoice objects to it, then
        give it some SampledSound objects to play.

        @see SamplerVoice, Synthesiser, SynthesiserSound

        @tags{Audio}
    */
    class JUCE_API  DrumSamplerSound    : public SynthesiserSound
    {
    public:
        //==============================================================================
        /** Creates a sampled sound from an audio reader.

            This will attempt to load the audio from the source into memory and store
            it in this object.

            @param name         a name for the sample
            @param source       the audio to load. This object can be safely deleted by the
                                caller after this constructor returns
            @param midiNotes    the set of midi keys that this sound should be played on. This
                                is used by the SynthesiserSound::appliesToNote() method
            @param midiNoteForNormalPitch   the midi note at which the sample should be played
                                            with its natural rate. All other notes will be pitched
                                            up or down relative to this one
            @param attackTimeSecs   the attack (fade-in) time, in seconds
            @param releaseTimeSecs  the release (fade-out) time, in seconds
            @param maxSampleLengthSeconds   a maximum length of audio to read from the audio
                                            source, in seconds
        */
        DrumSamplerSound (const String& name,
                      AudioFormatReader& source,
                      const BigInteger& midiNotes,
                      int midiNoteForNormalPitch,
                      double attackTimeSecs,
                      double releaseTimeSecs,
                      double maxSampleLengthSeconds);

        /** Destructor. */
        ~DrumSamplerSound() override;

        //==============================================================================
        /** Returns the sample's name */
        const String& getName() const noexcept                  { return name; }

        /** Returns the audio sample data.
            This could return nullptr if there was a problem loading the data.
        */
        AudioBuffer<float>* getAudioData() const noexcept       { return data.get(); }

        //==============================================================================
        /** Changes the parameters of the ADSR envelope which will be applied to the sample. */
        void setEnvelopeParameters (ADSR::Parameters parametersToUse)    { params = parametersToUse; }

        //==============================================================================
        bool appliesToNote (int midiNoteNumber) override;
        bool appliesToChannel (int midiChannel) override;
        
        friend class DrumSamplerSound;

        String name;
        std::unique_ptr<AudioBuffer<float>> data;
        double sourceSampleRate;
        BigInteger midiNotes;
        int length = 0, midiRootNote = 0;

        ADSR::Parameters params;

    private:
        //==============================================================================
        

        JUCE_LEAK_DETECTOR (DrumSamplerSound)
    };


    //==============================================================================
    /**
        A subclass of SynthesiserVoice that can play a SamplerSound.

        To use it, create a Synthesiser, add some SamplerVoice objects to it, then
        give it some SampledSound objects to play.

        @see SamplerSound, Synthesiser, SynthesiserVoice

        @tags{Audio}
    */
    class JUCE_API  DrumSamplerVoice    : public SynthesiserVoice
    {
    public:
        //==============================================================================
        /** Creates a SamplerVoice. */
        DrumSamplerVoice(GroovOliouAudioProcessor*);

        /** Destructor. */
        ~DrumSamplerVoice() override;

        //==============================================================================
        bool canPlaySound (SynthesiserSound*) override;

        void startNote (int midiNoteNumber, float velocity, SynthesiserSound*, int pitchWheel) override;
        void stopNote (float velocity, bool allowTailOff) override;

        void pitchWheelMoved (int newValue) override;
        void controllerMoved (int controllerNumber, int newValue) override;

        void renderNextBlock (AudioBuffer<float>&, int startSample, int numSamples) override;
        using SynthesiserVoice::renderNextBlock;
        
        GroovOliouAudioProcessor *processor;
        DrumStripeHandler *currDrumStripe=nullptr;

        
//        float attackRatio=0.0;
//        float releaseRatio=0.0;
//        double sourceSampleRate = 0;
//        float attackReleaseLevel = 0, attackDelta = 0, releaseDelta = 0;
//        bool isInAttack = false, isInRelease = false;
//        int playMode=0; //0=noteOnOff 1=oneshoot

    private:
        //==============================================================================
        double pitchRatio = 0;
        double sourceSamplePosition = 0;
        float velocity=0;
//        float lgain = 0, rgain = 0;

        ADSR adsr;

        JUCE_LEAK_DETECTOR (DrumSamplerVoice)
    };

} // namespace juce
