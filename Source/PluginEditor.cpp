/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//#include "FilterObjects.h"
//==============================================================================
DigitalFiltersAudioProcessorEditor::DigitalFiltersAudioProcessorEditor (DigitalFiltersAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
        setSize (350, 400);
    
    // TONE CONTROL GUI COMPONENTS
    toneControlFreqDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    toneControlFreqDial.setRange(200.0f, 20000.0f);
    toneControlFreqDial.setValue(5000.0f);
    toneControlFreqDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    toneControlFreqDial.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&toneControlFreqDial);
    
    toneControlFreqLabel.setText("Tone (R)", juce::dontSendNotification);
    toneControlFreqLabel.attachToComponent(&toneControlFreqDial, false);
    toneControlFreqDial.setSkewFactorFromMidPoint(1200);
    addAndMakeVisible(&toneControlFreqLabel);

    toneControlFreqValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "centreFreq", toneControlFreqDial);
    
    // VOLUME CONTROL GUI COMPONENTS
    volumeDial.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    volumeDial.setRange(0, 100000.0f);
    volumeDial.setValue(10000.0f);
    volumeDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeDial.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&volumeDial);

    volumeLabel.setText("Volume (R)", juce::dontSendNotification);
    volumeLabel.attachToComponent(&volumeDial, false);
    addAndMakeVisible(&volumeLabel);
    
    volumeValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "volume", volumeDial);
    
    
    
}

DigitalFiltersAudioProcessorEditor::~DigitalFiltersAudioProcessorEditor()
{
}

//==============================================================================
void DigitalFiltersAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void DigitalFiltersAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    volumeDial.setBounds(50, 150, 100, 100);
    toneControlFreqDial.setBounds(200, 150, 100, 100);
}

void DigitalFiltersAudioProcessorEditor::filterButtonClicked()
{

    
}
