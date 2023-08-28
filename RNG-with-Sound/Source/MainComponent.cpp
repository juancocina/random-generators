#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : startTime(juce::Time::getMillisecondCounterHiRes() * 0.001)
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 300);
    
    //setting up message box
    addAndMakeVisible(midiMessagesBox);
    midiMessagesBox.setMultiLine(true);
    midiMessagesBox.setReturnKeyStartsNewLine(true);
    midiMessagesBox.setReadOnly(true);
    midiMessagesBox.setScrollbarsShown(true);
    midiMessagesBox.setCaretVisible(false);
    midiMessagesBox.setPopupMenuEnabled(true);
    midiMessagesBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x32ffffff));
    midiMessagesBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
    midiMessagesBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));

    //midi Output List
    addAndMakeVisible(midiOutputList);
    midiOutputList.setTextWhenNoChoicesAvailable("No MIDI outputs detected");
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();

    juce::StringArray midiOutputNames;

    for (auto output : midiOutputs) midiOutputNames.add(output.name);
    midiOutputList.addItemList(midiOutputNames, 1);
    midiOutputList.onChange = [this] { setMidiOutput(midiOutputList.getSelectedItemIndex()); };

    // find the first enabled device and use it by default 
    for (auto output : midiOutputs) {
        if (deviceManager.isMidiInputDeviceEnabled(output.identifier)) {
            setMidiOutput(midiOutputs.indexOf(output));
            break;
        }
    }

    //setting up buttons
    addAndMakeVisible(generateButton);
    generateButton.setButtonText("Generate");
    generateButton.onClick = [this] { notes.clear(); generateNotes(); };

    /*addAndMakeVisible(noteOn);
    noteOn.setButtonText("Note On");
    noteOn.onClick = [this] {noteIsOn(50); };

    addAndMakeVisible(noteOff);
    noteOff.setButtonText("Note Off");
    noteOff.onClick = [this] {noteIsOff(50); };*/

    addAndMakeVisible(start);
    start.setButtonText("Start");
    start.onClick = [this] {startBuffer(); };

    addAndMakeVisible(stop);
    stop.setButtonText("Stop");
    stop.onClick = [this] {stopBuffer(); };

    //slider
    addAndMakeVisible(rateSlider);
    rateSlider.setRange(0.0, 1.0);
    rateSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    rateSlider.onValueChange = [this] { time = rateSlider.getValue(); };
    rateSlider.setValue(time);
    //rateSlider.setText("Rate", juce::dontSendNotification);

    addAndMakeVisible(noteSlider);
    noteSlider.setRange(1, 8, 1);
    noteSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    noteSlider.onValueChange = [this] { nAmount = noteSlider.getValue(); };



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
        setAudioChannels (0, 2);
    }

    //startTimer(1);
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

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()

}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
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
    //setting up midiMessagesBox
    auto halfWidth = getWidth() / 2;
    auto buttonsBounds = getLocalBounds().withWidth(halfWidth).reduced(100);
    midiMessagesBox.setBounds(getLocalBounds().withWidth(halfWidth).withX(halfWidth).reduced(10));
    midiOutputList.setBounds(buttonsBounds.getX(), 190, buttonsBounds.getWidth(), 20);
    midiOutputListLabel.setBounds(buttonsBounds.getX(), 220, buttonsBounds.getWidth(), 20);

    //buttons
    generateButton.setBounds(buttonsBounds.getX(), 10, buttonsBounds.getWidth(), 20);
    //noteOn.setBounds(buttonsBounds.getX(), 40, buttonsBounds.getWidth(), 20);
    //noteOff.setBounds(buttonsBounds.getX(), 70, buttonsBounds.getWidth(), 20);
    start.setBounds(buttonsBounds.getX(), 40, buttonsBounds.getWidth(), 20);
    stop.setBounds(buttonsBounds.getX(), 70, buttonsBounds.getWidth(), 20);

    //slider
    rateSlider.setBounds(buttonsBounds.getX(), 100, buttonsBounds.getWidth(), 20);
    noteSlider.setBounds(buttonsBounds.getX(), 130, buttonsBounds.getWidth(), 20);
}

//void MainComponent::timerCallback() { //sends the signal to play a note
//    auto currentTime = juce::Time::getMillisecondCounterHiRes() *0.001 - startTime;
//    auto currentSampleNumber = (int)(currentTime * sampleRate); 
//
//
//    for (const auto metadata : midiBuffer) {
//        if (metadata.samplePosition > currentSampleNumber) break;
//
//        auto message = metadata.getMessage();
//        message.setTimeStamp(metadata.samplePosition / sampleRate);
//        addMessageToList(message);
//        midiOutput->sendMessageNow(message);
//    }
//    midiBuffer.clear(previousSampleNumber, currentSampleNumber - previousSampleNumber);
//    previousSampleNumber = currentSampleNumber;
//
//    if (midiBuffer.isEmpty() && onOffSwitch) {
//        generateNotes();
//    }
//}

void MainComponent::timerCallback() { // come back in here an figure out how to iterate through the sequence
    auto currentTime = juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime;
    auto currentSampleNumber = (int)(currentTime * sampleRate); 
    
    
    for (const auto metadata : midiBuffer) {
        if (metadata.samplePosition > currentSampleNumber) break;
    
        auto message = metadata.getMessage();
        message.setTimeStamp(metadata.samplePosition / sampleRate);
        addMessageToList(message);
        midiOutput->sendMessageNow(message);
    }
    midiBuffer.clear(previousSampleNumber, currentSampleNumber - previousSampleNumber);
    previousSampleNumber = currentSampleNumber;
    
    if (midiBuffer.isEmpty() && onOffSwitch) {
        generateNotes();
    }
}


//void MainComponent::setNoteNumber(int noteNumber) {
//    auto message = juce::MidiMessage::noteOn(midiChannel, noteNumber, (juce::uint8)100);
//    message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime);
//    addMessageToBuffer(message);
//    //sendToOutput(message);
//    midiOutput->sendMessageNow(message);
//
//    auto messageOff = juce::MidiMessage::noteOff(midiChannel, noteNumber);
//    messageOff.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime+.015);
//    addMessageToBuffer(messageOff);
//    //sendToOutput(messageOff);
//    midiOutput->sendMessageNow(messageOff);
//
//}
void MainComponent::setNoteNumber(int noteNumber) {
    auto message = juce::MidiMessage::noteOn(midiChannel, noteNumber, (juce::uint8)100);
    message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime);
    addMessageToBuffer(message);
    
    auto messageOff = juce::MidiMessage::noteOff(message.getChannel(), noteNumber);
    messageOff.setTimeStamp(message.getTimeStamp() + 0.1);
    addMessageToBuffer(messageOff);
}
void MainComponent::noteIsOn(int noteNumber) {
    midiOutput->clearAllPendingMessages();
    auto message = juce::MidiMessage::noteOn(midiChannel, noteNumber, (juce::uint8)100);
    message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime);
    addMessageToBuffer(message);
    //sendToOutput(message);
    midiOutput->sendMessageNow(message);
}
void MainComponent::noteIsOff(int noteNumber) {
    midiOutput->clearAllPendingMessages();
    auto messageOff = juce::MidiMessage::noteOff(midiChannel, noteNumber);
    messageOff.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime);
    addMessageToBuffer(messageOff);
    //sendToOutput(messageOff);
    midiOutput->sendMessageNow(messageOff);
}

void MainComponent::addMessageToBuffer(const juce::MidiMessage& message) {
    auto timestamp = message.getTimeStamp();
    auto sampleNumber = (int)(timestamp * sampleRate);
    midiBuffer.addEvent(message, sampleNumber);
    midiSequence.addEvent(message);
    midiSequence.updateMatchedPairs();
}

void MainComponent::addMessageToList(const juce::MidiMessage& message) {
    auto time = message.getTimeStamp();

    auto hours = ((int)(time / 3600.0)) % 24;
    auto minutes = ((int)(time / 60.0)) % 60;
    auto seconds = ((int)time) % 60;
    auto millis = ((int)(time * 1000.0)) % 1000;

    auto timecode = juce::String::formatted("%02d:%02d:%02d.%03d",
        hours,
        minutes,
        seconds,
        millis);

    logMessage(timecode + " - " + getMidiMessageDescription(message));
}

juce::String MainComponent::getMidiMessageDescription(const juce::MidiMessage& m) {
    if (m.isNoteOn())           return "Note on " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
    if (m.isNoteOff())          return "Note off " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
    if (m.isProgramChange())    return "Program change " + juce::String(m.getProgramChangeNumber());
    if (m.isPitchWheel())       return "Pitch wheel " + juce::String(m.getPitchWheelValue());
    if (m.isAftertouch())       return "After touch " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3) + ": " + juce::String(m.getAfterTouchValue());
    if (m.isChannelPressure())  return "Channel pressure " + juce::String(m.getChannelPressureValue());
    if (m.isAllNotesOff())      return "All notes off";
    if (m.isAllSoundOff())      return "All sound off";
    if (m.isMetaEvent())        return "Meta event";

    if (m.isController())
    {
        juce::String name(juce::MidiMessage::getControllerName(m.getControllerNumber()));

        if (name.isEmpty())
            name = "[" + juce::String(m.getControllerNumber()) + "]";

        return "Controller " + name + ": " + juce::String(m.getControllerValue());
    }

    return juce::String::toHexString(m.getRawData(), m.getRawDataSize());

}

void MainComponent::logMessage(const juce::String& m) {
    midiMessagesBox.moveCaretToEnd();
    midiMessagesBox.insertTextAtCaret(m + juce::newLine);
}


// RNG functions ===========================
void MainComponent::generateNotes() { // !!! Will need to add parameters here
    if (notes.empty()) {
        for (int i = 0; i < nAmount; i++) {
            notes.push_back(juce::Random::getSystemRandom().nextInt(72) + 36);
        }

        
        /*notes.push_back(99);
        notes.push_back(73);
        notes.push_back(85);
        notes.push_back(66);*/
        //notes.push_back(58);
        //notes.push_back(90);
    
        /*notes.push_back(50);
        notes.push_back(40);
        notes.push_back(55);
        notes.push_back(48);*/
    }
    
    double multiplier = time;
    /* The timing between notes in a buffer is equal to time, taken from the slider
        But the timings between the buffer itself is off
    */
    
    
    for (auto v : notes) {
        auto message = juce::MidiMessage::noteOn(midiChannel, v, (juce::uint8)100);
        message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime + multiplier);
        addMessageToBuffer(message);
    
        auto messageOff = juce::MidiMessage::noteOff(message.getChannel(), v);
        messageOff.setTimeStamp(message.getTimeStamp() + 0.2);
        addMessageToBuffer(messageOff);
    
        multiplier += time; //this line makes the notes sequence out
    }
    
    //testGen();
}
void MainComponent::newNotes() {
    notes.clear();
    for (int i = 0; i < 4; i++) {
        notes.push_back(juce::Random::getSystemRandom().nextInt(juce::Range<int>(36, 72)));
    }
}

void MainComponent::setMidiOutput(int index) {
    //midiOutput->juce::MidiOutput::clearAllPendingMessages();
    auto list = juce::MidiOutput::getAvailableDevices();

    auto newOutput = list[index];

    midiOutput = juce::MidiOutput::openDevice(newOutput.identifier);
    //auto message = juce::MidiMessage::noteOn(1, 50, 1.0f);
    //midiOutput->juce::MidiOutput::sendMessageNow(message);

}

void MainComponent::startBuffer() {
    startTimer(time);
    onOffSwitch = true;
}

void MainComponent::stopBuffer() {
    stopTimer();
    onOffSwitch = false;
    //midiBuffer.clear();
}

void MainComponent::testGen() {
    auto message = juce::MidiMessage::noteOn(midiChannel, 50, (juce::uint8)100);
    message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime);
    addMessageToBuffer(message);

    auto messageOff = juce::MidiMessage::noteOff(message.getChannel(), 50);
    messageOff.setTimeStamp(message.getTimeStamp() + 0.1);
    addMessageToBuffer(messageOff);

    message = juce::MidiMessage::noteOn(midiChannel, 60, (juce::uint8)100);
    message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime + .2);
    addMessageToBuffer(message);

    messageOff = juce::MidiMessage::noteOff(message.getChannel(), 60);
    messageOff.setTimeStamp(message.getTimeStamp() + 0.1);
    addMessageToBuffer(messageOff);

    message = juce::MidiMessage::noteOn(midiChannel, 62, (juce::uint8)100);
    message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime + .4);
    addMessageToBuffer(message);

    messageOff = juce::MidiMessage::noteOff(message.getChannel(), 62);
    messageOff.setTimeStamp(message.getTimeStamp() + 0.1);
    addMessageToBuffer(messageOff);

    message = juce::MidiMessage::noteOn(midiChannel, 58, (juce::uint8)100);
    message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime + .6);
    addMessageToBuffer(message);

    messageOff = juce::MidiMessage::noteOff(message.getChannel(), 58);
    messageOff.setTimeStamp(message.getTimeStamp() + 0.1);
    addMessageToBuffer(messageOff);

}



//void MainComponent::generateNotes() { // !!! Will need to add parameters here
//    if (notes.empty()) {
//        notes.push_back(99);
//        notes.push_back(73);
//        notes.push_back(85);
//        notes.push_back(66);
//
//        /*notes.push_back(50);
//        notes.push_back(40);
//        notes.push_back(55);
//        notes.push_back(48);*/
//    }
//
//    double multiplier = time;
//    /* The timing between notes in a buffer is equal to time, taken from the slider
//        But the timings between the buffer itself is off
//    */
//
//
//    for (auto v : notes) {
//        auto message = juce::MidiMessage::noteOn(midiChannel, v, (juce::uint8)100);
//        message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime + multiplier);
//        addMessageToBuffer(message);
//
//        auto messageOff = juce::MidiMessage::noteOff(message.getChannel(), v);
//        messageOff.setTimeStamp(message.getTimeStamp() + 0.2);
//        addMessageToBuffer(messageOff);
//
//        multiplier += time;
//    }
//
//    //testGen();
//}
