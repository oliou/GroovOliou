//
//  DrumStripe.cpp
//  Groovoliou2
//
//  Created by oliou on 15/07/2015.
//
//

#include "DrumStripeHandler.h"
#include "PluginEditor.h"


DrumStripeHandler::DrumStripeHandler(JuceDemoPluginAudioProcessor* processor,
                                   //JuceDemoPluginAudioProcessorEditor* editor,
                                   //DirectoryContentsList* directoryContentsList,
                                   String sPath,
                                   int position,
                                   int note):
    // AudioProcessorEditor (owner),
    //mainEditor(editor),
    thread("Directory Contents Scanning Thread"),
    directoryContentsList(nullptr, thread),
    processor(processor),
    position(position),
    note(note),
    //directoryContentsList(directoryContentsList),
    samplesPath(sPath+(String)(position)+'/'),
    selectionStr(""),
    sampleLabel("", "sample"),
    attackLabel("", attackName),
    decayLabel("", decayName),
    pitchLabel("", pitchName),
    balanceLabel("", balanceName),
    volumeLabel("", volumeName),
    attackSlider(attackName),
    decaySlider(decayName),
    pitchSlider(pitchName),
    balanceSlider(balanceName),
    volumeSlider(volumeName),
    outputDropDown("Output Pair"),
    outputLabel("","Output:"),
    noteLabel("","Note: "+(String)note),
    selectFileButton("(Click here to select a sample)"),
    selectFileFolderButton("Change Folder"),
    playButton((String)(position)),
    nextSoundButton(">"),
    prevSoundButton("<"),
    playModeButton("PlayMode"),
    reverseButton("Reverse")
{
    debugLog((String) __FUNCTION__ + (String) position, true );
    
    directoryContentsList.addChangeListener(this);
    DrumStripeHandler::updateDirectoryContentList();
    
    outputDropDown.setTextWhenNothingSelected ("Output Pair");
    outputDropDown.addItem ("1,2", 1);
    outputDropDown.addItem ("3,4", 3);
    outputDropDown.addItem ("5,6", 5);
    outputDropDown.addItem ("7,8", 7);
    outputDropDown.addItem ("9,10", 9);
    outputDropDown.addItem ("11,12", 11);
    outputDropDown.addItem ("13,14", 13);
    outputDropDown.addItem ("15,16", 15);
    outputDropDown.setSelectedId(outputSelected);
    debugLog((juce::String)  "outputSelected:" + (juce::String) outputSelected);
    
    playButton.setColour(TextButton::buttonColourId, Colours::blanchedalmond);
    
    
//    playButton.setState(TextButton::buttonDown);
//    String test = directoryContentsList.getFile(0).getFileName();

//    
//    samplesPath ="~/Library/Audio/Plug-Ins/VST/GroovOliouRes/1";
//    String samplePath = samplesPath+"/BT0AAD0.WAV";
//    formatManager.registerBasicFormats();
//    File audioFile (samplePath);
//    ScopedPointer<AudioFormatReader> reader( formatManager.createReaderFor(audioFile));
//    BigInteger notes;
//    notes.setRange (note, 1, true);
//    SamplerSound::Ptr sound = new SamplerSound (samplePath, *reader, notes, note, 0.0, 0.1, 60.0);
//    processor->synth.addSound(sound);
//    selectionStr = "test";
    
    playButton.setName(playButtonName);
    
    startTimer (100);
};

DrumStripeHandler::~DrumStripeHandler(){};


void DrumStripeHandler::setSound(double newIndex){
    debugLog((String) __FUNCTION__, true );
    if(newIndex < directoryContentsList.getNumFiles()){
        formatManager.registerBasicFormats();
        ScopedPointer<AudioFormatReader> reader( formatManager.createReaderFor(directoryContentsList.getFile(newIndex)));
        
        String fileName = directoryContentsList.getFile(newIndex).getFileName();
        String filePath = samplesPath+fileName;
        
        if (reader != nullptr )
        {
            this->deleteSound();
            BigInteger notes;
            notes.setRange (note, 1, true);
            soundObj=nullptr;
            soundObj = new SamplerSound(filePath, *reader, notes, note, 0.0, 0.1, 60.0);
            processor->synth.addSound(soundObj);
            soundLoaded = true;
            soundIndex = newIndex;
            this->setLabel(fileName,false);
        }else{
            soundIndex++;
            soundLoaded = false;
            this->setSound(soundIndex);
        }
    }else{
        this->setLabel("No valid audio file found in folder.",false);
    }
}

bool DrumStripeHandler::hasThatSound(){
//    bool hasSound = false;
//    for (int i = 0; i < processor->synth.getNumSounds(); ++i)
//    {
//        if (processor->synth.getSound(i) == soundObj)
//        {
//            hasSound = true;
//            break;
//        }
//    }
//    return hasSound;
    return FALSE;
}

void DrumStripeHandler::updateDirectoryContentList(){
    String temp = "";
    directoryContentsList.setDirectory(temp, false, true);
    thread.startThread(1);
    directoryContentsList.setDirectory(samplesPath, false, true);
    thread.startThread(1);
}

void DrumStripeHandler::changeListenerCallback (ChangeBroadcaster* source)
{
    debugLog((String) __FUNCTION__ + (String) position, true );
    if(source == &directoryContentsList and directoryContentsList.getDirectory().getFullPathName() !="" ){
//        processor->info = processor->info + " changeListenerCallback ";
        this->updateSoundIndexFromSelection();
        if(!directoryContentsList.isStillLoading()){ //soundLoaded != true and 
            this->setSound(soundIndex);
        }
    }
}

void DrumStripeHandler::updateSoundIndexFromSelection(){
    debugLog((String) __FUNCTION__ + (String) position );
//    processor->info =  "Selection_: "+ selectionStr + "path_:"+samplesPath;
    if(selectionStr != ""){
        for (int i = 0; i < directoryContentsList.getNumFiles(); ++i)
        {
            if( selectionStr == directoryContentsList.getFile(i).getFileNameWithoutExtension()){
                soundIndex=i;
            }
        }
    }else{
        soundIndex = forcedSoundIndex;
        //forcedSoundIndex = 0;
        
    }
}

void DrumStripeHandler::loadNextSound(){
    soundLoaded = false;
    this->setSound(soundIndex+1);
}

void DrumStripeHandler::loadPrevSound(){
    soundLoaded = false;
    if(soundIndex > 0){
        this->setSound(soundIndex-1);
    }
}

void DrumStripeHandler::deleteSound(){
    for (int i = 0; i < processor->synth.getNumSounds(); ++i)
    {
        if( processor->synth.getSound(i) == soundObj )
        {
            processor->synth.removeSound(i);
            soundObj=nullptr;
            //mainEditor->setInfoMessage((String) i +"|", true);
            //this->setLabel(" idx:"+ (String) i ,true);
        }
    }
}

void DrumStripeHandler::flashPlayButton(bool forceFlash){
    
    if(forceFlash){
        playStarted = true;
    }
    
    if(playStarted){
        playButton.setColour(TextButton::buttonColourId, Colours::red);
        playStarted = false;
    }else{
        if(canUpdateNote){
            playButton.setColour(TextButton::buttonColourId, Colours::blueviolet);
        } else {
            playButton.setColour(TextButton::buttonColourId, Colours::blanchedalmond);
        }
    }
}

void DrumStripeHandler::timerCallback()
{
    flashPlayButton();
}

void DrumStripeHandler::setLabel(String message, bool append)
{
    if(!append){
        sampleLabel.setText(message, dontSendNotification);
    }else{
        sampleLabel.setText(sampleLabel.getText() + message, dontSendNotification);
    }
}

String DrumStripeHandler::getLabel()
{
    return sampleLabel.getText();
}

void DrumStripeHandler::refreshView(){
    int vShift = width*(position-1);
    int hStart = 50;
    int hShift = hStart;
    
    selectFileButton.setBounds (5+vShift, hShift , 95, 30);
    hShift = hShift+25;
    sampleLabel.setBounds (5+vShift, hShift , 100, 40);
    //    selectFileFolderButton.setBounds(10+vShift, hStart +sliderHeight/2 , 80, 20);
    hShift = hShift+35;
    nextSoundButton.setBounds (60+vShift, hShift , 20, 20);
    prevSoundButton.setBounds (15+vShift, hShift , 20, 20);
    hShift = hShift+10;
    noteLabel.setBounds (20+vShift, hShift , 50, 50);
    hShift = hShift+40;
    playButton.setBounds (23+vShift, hShift , 50, 50);
    hShift = hShift+60;
    playModeButton.setBounds (10+vShift, hShift, 100, 20);
    hShift = hShift+20;
    reverseButton.setBounds (10+vShift, hShift, 100, 20); hShift = hShift+40;
    attackSlider.setBounds (20+vShift, hShift , 60, 60); hShift = hShift+sliderHeight;
    decaySlider.setBounds (20+vShift, hShift , 60, 60); hShift = hShift+sliderHeight;
    pitchSlider.setBounds (20+vShift, hShift , 60, 60); hShift = hShift+sliderHeight;
    balanceSlider.setBounds (20+vShift, hShift , 60, 60); hShift = hShift+sliderHeight;
    volumeSlider.setBounds (20+vShift, hShift , 60, 60); hShift = hShift+sliderHeight;
    outputLabel.setBounds(10+vShift, hShift , width-20, 20); hShift = hShift+sliderHeight/4;
    outputDropDown.setBounds(15+vShift, hShift , width-30, 20);
    
    //sampleLabel.setText(directoryContentsList.getFile(0).getFileName(),dontSendNotification);
}

//void DrumStripeHandler::resize(){
//    
//}

void DrumStripeHandler::setEditor(JuceDemoPluginAudioProcessorEditor* editor){
    mainEditor = editor;
    this->setGUIComponents();
}

void DrumStripeHandler::setGUIComponents(){
    debugLog((String) __FUNCTION__  + (String) position, true );
    mainEditor->addAndMakeVisible(sampleLabel);
    this->initializeSlider(/*mainEditor,*/ &attackSlider, &attackLabel, 0, 1, 0.001,attack);
    this->initializeSlider(/*mainEditor,*/ &decaySlider, &decayLabel, 0, 1, 0.01,decay);
    this->initializeSlider(/*mainEditor,*/ &pitchSlider, &pitchLabel, 0, 2, 0.01,pitch);
    this->initializeSlider(/*mainEditor,*/ &balanceSlider, &balanceLabel, -1, 1, 0.01,balance);
    this->initializeSlider(/*mainEditor,*/ &volumeSlider, &volumeLabel, 0, 2, 0.01,volume);
    mainEditor->addAndMakeVisible(selectFileButton);
//    mainEditor->addAndMakeVisible(selectFileFolderButton);
    mainEditor->addAndMakeVisible(playButton);
    mainEditor->addAndMakeVisible(nextSoundButton);
    mainEditor->addAndMakeVisible(prevSoundButton);
    mainEditor->addAndMakeVisible(playModeButton);
    mainEditor->addAndMakeVisible(reverseButton);

    
    mainEditor->addKeyListener(this);

    outputDropDown.setSelectedId(outputSelected);
    mainEditor->addAndMakeVisible(outputLabel);
    mainEditor->addAndMakeVisible(outputDropDown);
    outputDropDown.addListener (this);

    noteLabel.setText("Note: "+(String)note, dontSendNotification);
    mainEditor->addAndMakeVisible(noteLabel);
    
    playModeButton.setToggleState((bool) playMode , sendNotification);
    this->togglePlayModeButton(playModeButton.getToggleState());
    
    reverseButton.setToggleState((bool) reverse , sendNotification);
    this->toggleReverseButton(reverseButton.getToggleState());
    
    debugLog((String) "playMode: "  + (String) playMode);
    
    attackSlider.addMouseListener(this,false);
    decaySlider.addMouseListener(this,false);
    pitchSlider.addMouseListener(this,false);
    balanceSlider.addMouseListener(this,false);
    volumeSlider.addMouseListener(this,false);
    
    
    
//    pitchSlider.addListener(this);
//    volumeSlider.addKeyListener(this);
    
    selectFileButton.addListener(this);
    selectFileFolderButton.addListener(this);
    playButton.addListener(this);
    playButton.addMouseListener(this,false);
    playButton.setTriggeredOnMouseDown(true);
    nextSoundButton.addListener(this);
    prevSoundButton.addListener(this);
    playModeButton.addListener(this);
    reverseButton.addListener(this);
    //playModeButton.triggerClick();
    
    
}

void DrumStripeHandler::initializeSlider(//JuceDemoPluginAudioProcessorEditor* mainEditor,
                                         juce::Slider *slider,
                                        Label *label,
                                        int min,
                                        int max,
                                        float step,
                                        float value)
{
    mainEditor->addAndMakeVisible(slider);
    slider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    //attackSlider.setRotaryParameters(Slider::RotaryHorizontalVerticalDrag, float_Pi, true)
    slider->setTextBoxStyle(Slider::TextBoxBelow, true, 40, 20 );
    slider->addListener(this);
    slider->setRange(min, max, step);
    slider->setValue( (double) value);
    label->attachToComponent (slider, false);
    label->setFont (Font (11.0f));
}


void DrumStripeHandler::togglePlayModeButton(bool toggleState){
    debugLog((String) __FUNCTION__  + (String) position, true );
    debugLog((String) "togglePlayModeButton(): toggleState: " + (String)toggleState);
    if(toggleState){ debugLog((String)"if");
        playMode=1;
        playModeButton.setButtonText("One Shot");
    }else{ debugLog((String)"else");
        playMode=0;
        playModeButton.setButtonText("Note On/Off");
    }
}

void DrumStripeHandler::toggleReverseButton(bool toggleState){
    debugLog((String) __FUNCTION__  + (String) position, true );
    debugLog((String) "toggleReverseButton(): toggleState: " + (String)toggleState);
    reverse = toggleState;
}

void DrumStripeHandler::buttonClicked(Button *button){
    
    if(button == &playButton){
        processor->synth.noteOn(1, note, 1);
        flashPlayButton(true);
    }
    if(button == &nextSoundButton){
        this->loadNextSound();
    }
    if(button == &prevSoundButton){
        this->loadPrevSound();
    }
    if(button == &playModeButton){
        this->togglePlayModeButton(playModeButton.getToggleState());
    }
    if(button == &reverseButton){
        this->toggleReverseButton(reverseButton.getToggleState());
    }
    if(button == &selectFileButton){
        FileChooser myChooser ("Please select an audio file...",  File(samplesPath),"*.wav,*.mp3");
        if(myChooser.browseForFileToOpen()){
            File selection (myChooser.getResult());
            selectionStr = selection.getFileNameWithoutExtension();//            processor->info = selection.getParentDirectory();
            soundIndex=0;
            samplesPath = selection.getParentDirectory().getFullPathName();
            DrumStripeHandler::updateDirectoryContentList();
        }
    }
    if(button == &selectFileFolderButton){
        FileChooser myChooser ("Please select the directory to use...",  File(samplesPath));
        if(myChooser.browseForDirectory()){
            File selection (myChooser.getResult());
            soundIndex=0;
            samplesPath = selection.getFullPathName();
            DrumStripeHandler::updateDirectoryContentList();
        }
    }
}


void DrumStripeHandler::mouseDoubleClick(const MouseEvent& event){
    debugLog((String) __FUNCTION__ );
//    processor->info = " mouseDoubleClick: "+ event.originalComponent->getName();
    if(event.originalComponent->getName() == volumeName){
        volumeSlider.setValue(volumeDefValue);
    }
    if(event.originalComponent->getName() == balanceName){
        balanceSlider.setValue(balanceDefValue);
    }
    if(event.originalComponent->getName() == pitchName){
        pitchSlider.setValue(pitchDefValue);
    }
    if(event.originalComponent->getName() == attackName){
        attackSlider.setValue(attackDefValue);
    }
    if(event.originalComponent->getName() == decayName){
        decaySlider.setValue(decayDefValue);
    }
    if(event.originalComponent->getName() == playButtonName){
        canUpdateNote = false;
        flashPlayButton();
    }
}

void DrumStripeHandler::mouseEnter(const juce::MouseEvent &event){
    
    debugLog((String) event.originalComponent->getName() );;
    
    if(event.originalComponent->getName() == volumeName
        || event.originalComponent->getName() == balanceName
        || event.originalComponent->getName() == pitchName
        || event.originalComponent->getName() == attackName
        || event.originalComponent->getName() == decayName){
        processor->info = sliderHelpMesg;
        debugLog((String) event.originalComponent->getName() );
    }
    if(event.originalComponent->getName() == playButtonName){
        processor->info = playButtonHelpMesg;
    }
}

void DrumStripeHandler::mouseExit(const juce::MouseEvent &event){
    if(event.originalComponent->getName() == volumeName
        || event.originalComponent->getName() == balanceName
        || event.originalComponent->getName() == pitchName
        || event.originalComponent->getName() == attackName
        || event.originalComponent->getName() == decayName
        || event.originalComponent->getName() == playButtonName
       ){
        processor->info = "";
    }
}

void DrumStripeHandler::updateNote(int newNote){
    if(canUpdateNote){
        note = newNote;
        //reset the sound with same soundIndex to pickup updated note.
        setSound(soundIndex);
        // Refresh Label
        noteLabel.setText("Note: "+(String)note, dontSendNotification); 
        canUpdateNote = false;
    }
    if(note == newNote){
        flashPlayButton(true);
    }
}

void DrumStripeHandler::mouseDown(const juce::MouseEvent & event){
    debugLog((String) __FUNCTION__ );
    ModifierKeys mKey = ModifierKeys::getCurrentModifiers();

    if((event.originalComponent->getName() == volumeName
    || event.originalComponent->getName() == balanceName
    || event.originalComponent->getName() == pitchName
    || event.originalComponent->getName() == attackName
    || event.originalComponent->getName() == decayName) && mKey.isCtrlDown()){
        processor->resetSlidersColour();
        processor->controlBindingMap[128]=(String)position+event.originalComponent->getName();
        event.originalComponent->setColour(Slider::rotarySliderFillColourId, Colours::blueviolet);
    }
    if(event.originalComponent->getName() == playButtonName && mKey.isCtrlDown()){
        canUpdateNote = true;
        flashPlayButton();
    }
}

void DrumStripeHandler::resetSlidersColour(){
    volumeSlider.setColour(Slider::rotarySliderFillColourId, Colours::blue);
    balanceSlider.setColour(Slider::rotarySliderFillColourId, Colours::blue);
    pitchSlider.setColour(Slider::rotarySliderFillColourId, Colours::blue);
    attackSlider.setColour(Slider::rotarySliderFillColourId, Colours::blue);
    decaySlider.setColour(Slider::rotarySliderFillColourId, Colours::blue);
}

void DrumStripeHandler::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &outputDropDown)
    {
        outputSelected = outputDropDown.getSelectedId();
    }
}

void DrumStripeHandler::handleControlChange(int controllerNumber, int controllerValue)
{
    if( processor->controlBindingMap[128] !="" ){
        processor->controlBindingMap.erase(controllerNumber);
        processor->controlBindingMap[controllerNumber]= processor->controlBindingMap[128];
    }
    
    debugLog((juce::String)  "ControlChange:" + (juce::String) controllerNumber + " | "+ (juce::String) controllerValue );

    
    processor->controlBindingMap[128]= "";
    for (auto it=processor->controlBindingMap.begin(); it!=processor->controlBindingMap.end(); ++it){
        if(it->first == controllerNumber){
            if(it->second == (String)position+volumeName){
                double newValue = 2 * (double) controllerValue/127.0;
                volumeSlider.setValue( newValue, sendNotificationSync);
            }
            if(it->second == (String)position+balanceName){
                double newValue = 1.02 * (double) (controllerValue-63)/64.0;
//                processor->info = "#potVal: " + (String) newValue;
                balanceSlider.setValue( newValue, sendNotificationSync);
            }
            if(it->second == (String)position+pitchName){
                double newValue = 2 * (double) controllerValue/127.0;
                pitchSlider.setValue( newValue, sendNotificationSync);
            }
            if(it->second == (String)position+attackName){
                double newValue = (double) controllerValue/127.0;
                attackSlider.setValue( newValue, sendNotificationSync);
            }
            if(it->second == (String)position+decayName){
                double newValue = (double) controllerValue/127.0;
                decaySlider.setValue( newValue, sendNotificationSync);
            }
            processor->resetSlidersColour();
        }
    }
}

bool DrumStripeHandler::keyPressed(const juce::KeyPress & key, juce::Component *originatingComponent)
{
//    processor->info = "key:" + (String) key.getKeyCode();
}

bool DrumStripeHandler::keyStateChanged(bool isKeyDown, juce::Component *originatingComponent)
{
    

}

//
//void DrumStripeHandler::buttonStateChanged(Button *button){
//    
//}

//void DrumStripeHandler::timerCallback()
//{
//    
//    
//}
//
//void DrumStripeHandler::paint (Graphics& g)
//{
//    
//}
//
//
//void DrumStripeHandler::resized()
//{
//    
//}
//
void DrumStripeHandler::sliderValueChanged (Slider* slider)
{
    if(slider == &balanceSlider){
        balance = slider->getValue();
    }
    if(slider == &volumeSlider){
        volume = slider->getValue();
    }
    if(slider == &pitchSlider){
        pitch = slider->getValue();
    }
    if(slider == &attackSlider){
        attack = slider->getValue();
    }
    if(slider == &decaySlider){
        decay = slider->getValue();
    }
}
//
//void DrumStripeHandler::sliderDragStarted (Slider* slider)
//{
// 
//}
//
//void DrumStripeHandler::sliderDragEnded (Slider* slider)
//{
//   
//}
//
//
//
//
