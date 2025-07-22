#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
		setAudioChannels(0, 2);    // params in channels, out  channels
    }
    juce::Component::addAndMakeVisible(playButton);
	juce::Component::addAndMakeVisible(stopButton);
	juce::Component::addAndMakeVisible(loadButton);
    juce::Component::addAndMakeVisible(volSlider);
    juce::Component::addAndMakeVisible(tempoSlider);
    
	playButton.addListener(this);
	stopButton.addListener(this);
    volSlider.addListener(this);
    tempoSlider.addListener(this);
	loadButton.addListener(this);
    
    // Set the initial value and range for the volume slider
    volSlider.setRange(0.0, 10.0, 0.01); // Range from 0.0 to 10.0 with step size of 0.01
    volSlider.setValue(0.0); // Set initial value to 0.0 (muted)
    
    // Set the slider style and appearance
    volSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    
	tempoSlider.setRange(0, 8, 0.01); // Range from -8 to 8 with step size of 0.01
	tempoSlider.setValue(1); // Set initial value to 1.0 (normal speed)
}                      

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

	phase = 0.0; // Initialize phase for audio processing
    dPhase = 0.0001; // Set delta phase for audio processing

	formatManager.registerBasicFormats(); // Register basic audio formats (WAV, MP3, etc.)
	
	
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{   
    // Your audio-processing code goes here!

	resampleSource.getNextAudioBlock(bufferToFill); // Get the next audio block from the transport source
    

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
     
    // auto* leftChan = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    // auto* rightChan = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    // for (auto i = 0; i < bufferToFill.numSamples; ++i)
    // {
    //     // Fill the left channel with random values
    //     //float sample = rand.nextFloat() * 0.15; // Random float between -1.0 and 1.0
    //     //float sample = (float)std::sin(phase); // Generate a sine wave sample
    //     //double sample = fmod(phase,0.2); // Generate a sawtooth wave sample
    //     double sample = std::sin(phase * 2.0 * juce::MathConstants<double>::pi); // Generate a sine wave sample
    //     leftChan[i] = sample;
    //     rightChan[i] = sample; // Copy the same value to the right channel for mono output
    //     phase += dPhase; // Increment phase for next sample
// 


    //bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
	transportSource.releaseResources(); // Release resources used by the transport sourceportSource.
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    // DEBUGGER CODE
    DBG("MainComponent::resized");

    // button test
    double rowH = juce::Component::getHeight() / 5.0; // Use 5.0 instead of 5 to ensure floating-point division
    double widthF = juce::Component::getWidth();
    playButton.juce::Component::setBounds(0, 0, widthF, rowH);
	stopButton.juce::Component::setBounds(0, rowH , widthF, rowH);
    volSlider.juce::Component::setBounds(0, rowH * 2.0, widthF, rowH );
    tempoSlider.juce::Component::setBounds(0, rowH * 3.0, widthF, rowH);
	loadButton.juce::Component::setBounds(0, rowH * 4.0, widthF, rowH);
}


void MainComponent::buttonClicked(juce::Button* button) {
    DBG("MainComponent::buttonClicked");
    // This is called when the user clicks the button.
	if (button == &playButton) {
		// Handle play button click
		DBG("Play Button Clicked");
		// You can start playback here
		if (!transportSource.isPlaying()) {
			transportSource.start(); // Start playback of the audio source
			DBG("Playback started.");
		}
		else {
			DBG("Playback is already in progress.");
		}

	}
	else if (button == &stopButton) {
        DBG("Stop Button Clicked");
		// Handle stop button click
		if (!transportSource.isPlaying()) {
			DBG("Playback is already stopped.");
		}
		else {	
			transportSource.stop(); // Stop playback of the audio source
			DBG("Playback stopped.");
		}
        //auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles;
        ////- launch out of the main thread
        ////- note how we use a lambda function which you've probably
        //// not seen before. Please do not worry too much about that
        //// but it is necessary as of JUCE 6.1
        //fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
        //    {
        //        auto chosenFile = chooser.getResult();
        //        auto* reader = formatManager.createReaderFor(chosenFile);
        //        std::unique_ptr<juce::AudioFormatReaderSource> newSource
        //        (new juce::AudioFormatReaderSource(reader, true));
        //        transportSource.setSource(newSource.get(), 0,
        //            nullptr, reader->sampleRate);
        //        readerSource.reset(newSource.release());
        //    }
        //);
    }
    else if (button == &loadButton) {
        // Handle load button click
        DBG("Load Button Clicked");
        // You can open a file chooser dialog to select an audio file here
        auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles;
        fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser) {
            auto chosenFile = chooser.getResult();
            if (chosenFile.existsAsFile()) {
                loadURL(juce::URL{ chosenFile });
            }
            else {
                DBG("No file selected or file does not exist.");
            }
            });
    }
	else {
		DBG("Unknown button clicked");
	}
}


void MainComponent::sliderValueChanged(juce::Slider* slider) {
	DBG("MainComponent::sliderValueChanged");
	// This is called when the user changes the value of the slider.
	if (slider == &volSlider) {
		// Handle volume slider changes
		DBG("Volume Slider Value: " << slider->getValue());
		// Set the volume of the transport source based on the slider value
		transportSource.setGain(static_cast<float>(slider->getValue())); // Set the gain of the transport source
		//dPhase = volSlider.getValue() * 0.006; // Adjust delta phase based on slider value
	}
    else if (slider == &tempoSlider) {
        // Handle volume slider changes
        DBG("Tempo Slider Value: " << slider->getValue());
		// Adjust the playback speed of the transport source based on the slider value
        resampleSource.setResamplingRatio(slider->getValue()); // Set the resampling ratio based on the slider value
        // Set the resampling ratio based on the slider value
    }

}

void MainComponent::loadURL(const juce::URL& audioURL) {
	// This function loads an audio file from a URL and prepares it for playback.
	auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
	if (reader != nullptr) {
		std::unique_ptr<juce::AudioFormatReaderSource> newReaderSource(new juce::AudioFormatReaderSource(reader, true));
		transportSource.setSource(newReaderSource.get(), 0, nullptr, reader->sampleRate);
		readerSource.reset(newReaderSource.release());
		//transportSource.start();
		DBG("Audio reader created successfully for the specified file.");
	}
	else {
		DBG("Failed to create audio reader for the specified file.");
	}
}

