/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
//==============================================================================
/**
*/
class DigitalFiltersAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DigitalFiltersAudioProcessorEditor (DigitalFiltersAudioProcessor&);
    ~DigitalFiltersAudioProcessorEditor() override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void filterButtonClicked();
    
private:
    
    juce::TextButton filterButton;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DigitalFiltersAudioProcessor& audioProcessor;
    
    juce::Slider toneControlFreqDial;
    juce::Slider gainDial;
    juce::Label toneControlFreqLabel;
    juce::Label gainLabel;
    //juce::TextButton convolutionButton;
    //juce::TextButton distortionButton;
    
    juce::Slider volumeDial;
    juce::Label volumeLabel;
    
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> toneControlFreqValue;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> gainValue;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> volumeValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DigitalFiltersAudioProcessorEditor)

};
