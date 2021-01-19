/*
  ==============================================================================

    Distortion.cpp
    Created: 6 Oct 2020 10:08:34pm
    Author:  Richie Haynes

  ==============================================================================
*/
#include "../JuceLibraryCode/JuceHeader.h"
#include "Distortion.h"
#include <JuceHeader.h>

Distortion::Distortion()
{
    auto& waveshaper = processorChain.template get<waveshaperIndex>();
    waveshaper.functionToUse = [&, this] (float x)

    {
        //return (std::tanh(pow(10, this->getGain() / 20) * x))/std::tanh(pow(10, this->getGain() / 20));
        
    
        
        //return tanh(x);
        
        return tanh(pow(10, this->getGain() / 20) * x);
        
        //return juce::jlimit (float (-0.95), float (0.95), x);

    };
    
    mOversampling.reset(new juce::dsp::Oversampling<float>(1, 3, juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple, false));

    auto& preGain = processorChain.template get<preGainIndex>();
    preGain.setGainDecibels (30.0f);
    
    auto& postGain = processorChain.template get<postGainIndex>();
    postGain.setGainDecibels (-20.0f);
}

//==============================================================================
void Distortion::prepare (const juce::dsp::ProcessSpec& spec)
{
    auto& filter = processorChain.template get<filterIndex>();
    //filter.state = FilterCoefs::makeFirstOrderLowPass (spec.sampleRate, 3500.0f);
    filter.state = FilterCoefs::makeFirstOrderHighPass(spec.sampleRate, 10.0f);
    processorChain.prepare (spec);
    
    mMaxBlockSize = spec.maximumBlockSize;
    
    mOversampling->initProcessing(static_cast<size_t>(mMaxBlockSize));

    reset();
}

//==============================================================================
//template <typename ProcessContext>
void Distortion::process (juce::dsp::ProcessContextReplacing<float> context) noexcept
{
    juce::ScopedNoDenormals noDenormals;
    // Create a block for oversampled, upsampled, data
    juce::dsp::AudioBlock<float> oversampledBlock = mOversampling -> processSamplesUp(context.getInputBlock());
    // Create context variable for upsampled data block
    auto waveshaperContext = juce::dsp::ProcessContextReplacing<float>(oversampledBlock);
    // Process oversampled block's context
    processorChain.template get<waveshaperIndex>().process(waveshaperContext);
    // Scale new incoming data blocks
    waveshaperContext.getOutputBlock() *= 0.7;
    
    
    // Downsample to original context
    mOversampling->processSamplesDown(context.getOutputBlock());
    // Process the distortion block
    processorChain.process (context);
    
}

//==============================================================================
void Distortion::reset() noexcept
{
    mOversampling->reset();
    processorChain.reset();
}

void Distortion::setGain(float gainValue)
{
    gain = gainValue;
}

float Distortion::getGain()
{
    return gain;
}
