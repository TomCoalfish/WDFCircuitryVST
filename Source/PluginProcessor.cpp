/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>
#include "FilterObjects.h"

//==============================================================================
DigitalFiltersAudioProcessor::DigitalFiltersAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),

tree(*this, nullptr, "Parameter",
{
    std::make_unique<juce::AudioParameterFloat> ("centreFreq", "CentreFreq", juce::NormalisableRange<float>(200.0f, 20000.0f, 0.1f, 0.6f), 5000.0f),

    std::make_unique<juce::AudioParameterFloat> ("gain", "Gain", 0.0f, 48.0f, 0.0f),

    std::make_unique<juce::AudioParameterFloat> ("volume", "Volume", juce::NormalisableRange<float> (0.0f, 10000.0f, 1.0f, 0.30f), 1000.0f)   } )





#endif
{
}

DigitalFiltersAudioProcessor::~DigitalFiltersAudioProcessor()
{
}

//==============================================================================
const juce::String DigitalFiltersAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DigitalFiltersAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DigitalFiltersAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DigitalFiltersAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DigitalFiltersAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DigitalFiltersAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DigitalFiltersAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DigitalFiltersAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DigitalFiltersAudioProcessor::getProgramName (int index)
{
    return {};
}

void DigitalFiltersAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DigitalFiltersAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    // Set up the DSP module ready for processing
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getMainBusNumOutputChannels();

    lowPassFilter.prepare(spec);
    lowPassFilter.reset();

    preGainCircuit[0].createWDF();
    preGainCircuit[1].createWDF();
    
    preGainCircuit[0].reset(sampleRate);
    preGainCircuit[1].reset(sampleRate);
    
    postGainCircuit[0].createWDF();
    postGainCircuit[1].createWDF();
    
    postGainCircuit[0].reset(sampleRate);
    postGainCircuit[1].reset(sampleRate);
    

}

void DigitalFiltersAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DigitalFiltersAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void DigitalFiltersAudioProcessor::updateFilter ()
{
    float centreFreq = *tree.getRawParameterValue("centreFreq");
    float volume = *tree.getRawParameterValue("volume");
    float gain = *tree.getRawParameterValue("gain");
    
    for (int channel = 0; channel < getTotalNumInputChannels(); channel++) {
        postGainCircuit[channel].setTone(centreFreq);
        
        postGainCircuit[channel].setVolume(volume);
        postGainCircuit[channel].reset(getSampleRate());
        postGainCircuit[channel].createWDF();

    }
    
}

void DigitalFiltersAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; channel++)
    {
        
        const float * inputBuffer = buffer.getReadPointer(channel);
        auto * outputData = buffer.getWritePointer(channel);
        
        
        
        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {

            float inputSample = inputBuffer[sample];
            
            double preGainCircuitOut = preGainCircuit[channel].processAudioSample(inputSample);
            
            double postGainCircuitOut = postGainCircuit[channel].processAudioSample(preGainCircuitOut);
            
            outputData[sample] = (float)postGainCircuitOut;
            
            updateFilter();

        }
        
        

    }

}


//==============================================================================
bool DigitalFiltersAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DigitalFiltersAudioProcessor::createEditor()
{
    return new DigitalFiltersAudioProcessorEditor (*this);
}

//==============================================================================
void DigitalFiltersAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DigitalFiltersAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DigitalFiltersAudioProcessor();
}
