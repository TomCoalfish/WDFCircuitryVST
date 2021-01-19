/*
  ==============================================================================

    Distortion.h
    Created: 6 Oct 2020 10:08:25pm
    Author:  Richie Haynes

  ==============================================================================
*/
#include <JuceHeader.h>
#include "../JuceLibraryCode/JuceHeader.h"
#pragma once

class Distortion
{
public:
    Distortion();
    float gain;
    void prepare (const juce::dsp::ProcessSpec& spec);
    //template <typename ProcessContext>
    void process (juce::dsp::ProcessContextReplacing<float> context) noexcept;
    void reset() noexcept;
    void setGain(float gainValue);
    float getGain();
    

private:
    //==============================================================================
    enum
    {
        //filterIndex,
        preGainIndex,
        waveshaperIndex,
        postGainIndex
    };
    
    std::unique_ptr<juce::dsp::Oversampling<float>> mOversampling;
    juce::uint32 mMaxBlockSize = 512;
    
    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;
    
    /*juce::dsp::ProcessorChain<juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>,
                             juce::dsp::Gain<float>, juce::dsp::WaveShaper<float, std::function<float (float)>>, juce::dsp::Gain<float>> processorChain;*/
    
    juce::dsp::WaveShaper<float> waveshaper;
    
    juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::WaveShaper<float, std::function<float (float)>>, juce::dsp::Gain<float>> processorChain;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Distortion)
    
};
