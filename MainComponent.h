#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent
	                , public juce::Button::Listener
	                , public juce::Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button*) override;
    void sliderValueChanged(juce::Slider*) override;
	
	//==============================================================================
	// This is where you can add your own methods and member variables.
	// For example, you might want to add a method to set the volume of the audio output,
	// or to change the sound being played.
    
private:
    //==============================================================================
    // Your private member variables go here...
    juce::TextButton playButton{ "PLAY" };
    juce::TextButton stopButton{ "STOP" };
    juce::TextButton loadButton{ "LOAD" };

    juce::Slider volSlider;
    juce::Slider tempoSlider;

	juce::Random rand; // Random number generator for creating audio data

	double phase; // Phase for audio processing
	double dPhase; // Delta phase for audio processing

	juce::AudioFormatManager formatManager; // Manages audio formats
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource; // Reads audio data from a file
	juce::AudioTransportSource transportSource; // Handles audio playback
    juce::ResamplingAudioSource resampleSource{ &transportSource, false, 2}; // 

    juce::FileChooser fChooser{ "Select a file..." };

	void loadURL(const juce::URL& audioURL); // Loads an audio file from a URL

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
