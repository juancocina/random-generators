#pragma once

#include <JuceHeader.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <random> //rng 
#include <chrono>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

class MainComponent  : public juce::AudioAppComponent, private juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    void generateNotes();
    void newNotes();
    void setMidiOutput(int index);
    void noteIsOn(int noteNumber);
    void noteIsOff(int noteNumber);
    void startBuffer();
    void stopBuffer();
    void testGen();

   /* void sendToOutput(const juce::MidiMessage& m);*/
 
private:
    //==============================================================================
    // functions
    void timerCallback() override;
    void setNoteNumber(int noteNumber);
    void addMessageToBuffer(const juce::MidiMessage& message);
    void addMessageToList(const juce::MidiMessage& message);
    static juce::String getMidiMessageDescription(const juce::MidiMessage& m);
    void logMessage(const juce::String& m);


    //variables
    int midiChannel = 1;
    double startTime;
    
    //setting up the midi message box
    juce::TextEditor midiMessagesBox;

    //midi 
    juce::MidiBuffer midiBuffer;
    double sampleRate = 44100.0;
    int previousSampleNumber = 0; 

    juce::MidiMessageSequence midiSequence;

    juce::AudioDeviceManager deviceManager;
    juce::ComboBox midiOutputList;
    juce::Label midiOutputListLabel;

    std::unique_ptr<juce::MidiOutput> midiOutput;
    

    //buttons 
    juce::TextButton generateButton;
    juce::TextButton noteOn;
    juce::TextButton noteOff;
    juce::TextButton start;
    juce::TextButton stop;

    //sliders
    juce::Slider rateSlider;
    bool onOffSwitch = false;

    //storage for rng
    std::vector<int> notes;
    double time = 0.5;
    //double multiplier;

    //variables for random number generator
    juce::Slider noteSlider;
    int nAmount = 1; // needs to be at least 1




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
