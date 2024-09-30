/*
  ==============================================================================

    DeckGUI.cpp
    Created: 13 Mar 2020 6:44:48pm
    Author:  matthew

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player, 
                AudioFormatManager & 	formatManagerToUse,
                AudioThumbnailCache & 	cacheToUse,
                PlaylistComponent* playlistComponent
           ) : player(_player), 
               waveformDisplay(formatManagerToUse, cacheToUse),
               _playlistComponent(playlistComponent)
{
    addAndMakeVisible(nowPlayingLabel);
    nowPlayingLabel.setText("Now playing: -", dontSendNotification);

    addAndMakeVisible(currentTimeLabel);
    currentTimeLabel.setText("0:00", dontSendNotification);

    addAndMakeVisible(totalTimeLabel);
    totalTimeLabel.setText("/ 0:00", dontSendNotification);

    addAndMakeVisible(playButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(ffButton);
    addAndMakeVisible(resButton);
       
    addAndMakeVisible(volSlider);
    volSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    volSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 24);
    volSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::greenyellow.withAlpha(0.5f));
    volSlider.setDoubleClickReturnValue(true, 50);

    addAndMakeVisible(speedSlider);
    speedSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 24);
    speedSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::red.withAlpha(0.5f));
    speedSlider.setDoubleClickReturnValue(true, 1);

    addAndMakeVisible(posSlider);
    posSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    addAndMakeVisible(waveformDisplay);

    playButton.addListener(this);
    loopButton.addListener(this);
    loadButton.addListener(this);
    ffButton.addListener(this);
    resButton.addListener(this);

    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);

    volSlider.setRange(0, 100, 1);
    volSlider.setValue(50);
    speedSlider.setRange(1, 10, 0.25);
    posSlider.setRange(0.0, 1.0);

    startTimer(50);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll(Colours::black);  // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont(12.0f);
    g.drawText("Otodecks", getLocalBounds(),
        Justification::centred, true);   // draw some placeholder text
}

void DeckGUI::resized()
{
    double rowH = getHeight() / 9;
    double rowW = getWidth() / 11;
    double dialSize = getWidth() * 0.35;
    double maxdialSize = 210;

    if (dialSize > maxdialSize)
    {
        dialSize = maxdialSize;
    }

    nowPlayingLabel.setBounds(2, 5, getWidth(), rowH * 0.5);
    currentTimeLabel.setBounds(getWidth() * 0.8, 5, 60, rowH * 0.5);
    totalTimeLabel.setBounds(getWidth() * 0.88, 5, 60, rowH * 0.5);
    waveformDisplay.setBounds(5, (rowH/2)+8, getWidth()-10, rowH * 2.5);
    posSlider.setBounds(0, rowH * 2.9, getWidth(), rowH);
    volSlider.setBounds(rowW * 1.5, rowH * 3.5, dialSize, dialSize);
    speedSlider.setBounds(rowW * 6.5, rowH * 3.5, dialSize, dialSize);
    playButton.setBounds(getWidth()/3, rowH * 7, getWidth()/3, rowH);
    resButton.setBounds(getWidth()/5.1, rowH * 7.15, getWidth()/7.2, rowH*0.8);
    ffButton.setBounds(getWidth()/6*4, rowH * 7.15, getWidth()/8, rowH*0.8);
    loopButton.setBounds(getWidth()/6*5, rowH * 7, getWidth()/6, rowH);
    loadButton.setBounds(0, rowH * 8.28, getWidth(), rowH/1.3);
}

void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playButton)
    {
        if (fileIsLoaded == false)
        {
            AlertWindow::showMessageBoxAsync(
                AlertWindow::WarningIcon,
                "No Song Loaded",
                "Please load a song before playing.");
        }
        else if (playButton.getButtonText() == "Play")
        {
            playButton.setButtonText("Stop");
            player->start();
        }
        else
        {
            playButton.setButtonText("Play");
            player->stop();
        }
    }
    if (button == &ffButton)
    {
        double newPosition = player->getCurrentPosition() + 5.0;
        player->setPosition(newPosition);
    }
    if (button == &loadButton)
    {
        File pathFile = File::getCurrentWorkingDirectory().getChildFile("current_url.txt");
        if (pathFile.existsAsFile())
        {
            String path = pathFile.loadFileAsString().trim();
            File file(path);
            if (file.existsAsFile())
            {
                String path = pathFile.loadFileAsString();
                path = "file:///" + path.replace("\\", "/");
                player->loadURL(path);
                waveformDisplay.loadURL(path);
                fileIsLoaded = true;
                posSlider.setValue(0);
                //Display track time & update button
                double totalLength = player->getTotalLength();
                String totalLengthStart = formatTime(totalLength, 2);
                totalTimeLabel.setText("/ " + totalLengthStart, dontSendNotification);
                playButton.setButtonText("Play");
            }
            else
            {
                AlertWindow::showMessageBoxAsync(
                    AlertWindow::WarningIcon,
                    "File Not Found",
                    "The file specified in the path file was not found.");
            }
            nowPlayingLabel.setText("Now playing: " + file.getFileName(), dontSendNotification);
        }
        else
        {
            AlertWindow::showMessageBoxAsync(
                AlertWindow::WarningIcon,
                "No Song Selected",
                "Please select a song from the song list, or drag a song into a player.");
        }
    }
    if (button == &resButton)
    {
        if (fileIsLoaded) {
            posSlider.setValue(0);
            player->start();
            playButton.setButtonText("Stop");
        }
        else 
        {
            return;
        }
    }
}

    // if (button == &loadButton)
    // {
    //     FileChooser chooser{"Select a file..."};
    //     if (chooser.browseForFileToOpen())
    //     {
    //         player->loadURL(URL{chooser.getResult()});
    //         waveformDisplay.loadURL(URL{chooser.getResult()}); 
    //     }
    // }

void DeckGUI::sliderValueChanged (Slider *slider)
{
    if (slider == &volSlider)
    {
        double gainVal = slider->getValue() / 100;
        player->setGain(gainVal);
    }

    if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue());
    }
    
    if (slider == &posSlider)
    {
        player->setPositionRelative(slider->getValue());
    }
    
}

bool DeckGUI::isInterestedInFileDrag (const StringArray &files)
{
  std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl;
  return true; 
}

void DeckGUI::filesDropped (const StringArray &files, int x, int y)
{
  std::cout << "DeckGUI::filesDropped" << std::endl;
  if (files.size() == 1)
  {
    player->loadURL(URL{File{files[0]}});
    waveformDisplay.loadURL(URL{File{files[0]}});
    fileIsLoaded = true;

    // Create the 'tracks' folder if it does not exist
    File tracksFolder = File::getCurrentWorkingDirectory().getChildFile("tracks");
    if (!tracksFolder.exists())
        tracksFolder.createDirectory();

    // Get the file name and extension of the dropped file
    File droppedFile(files[0]);
    String fileName = droppedFile.getFileNameWithoutExtension();
    String fileExtension = droppedFile.getFileExtension();

    // Create a new file in the 'tracks' folder with the same name and extension as the dropped file
    File newFile = tracksFolder.getChildFile(fileName + fileExtension);

    double totalLength = player->getTotalLength();
    String totalLengthStart = formatTime(totalLength, 2);
    totalTimeLabel.setText("/ " + totalLengthStart, dontSendNotification);
    nowPlayingLabel.setText("Now playing: " + fileName + fileExtension, dontSendNotification);

    // Check if the new file already exists in the 'tracks' folder
    if (newFile.exists())
    {
        // Ask the user if they want to overwrite the file
        if (AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
            "Overwrite File",
            "A file with the same name already exists in folder. Do you want to overwrite it?",
            "OK", "Cancel") == 0)
        {
            newFile.deleteFile();
        }
        else
        {
            return;
        }
    }
    // Copy the dropped file to the new file in the 'tracks' folder
    droppedFile.copyFileTo(newFile);
    _playlistComponent->updateTrackTitles();
    _playlistComponent->loadTracks();
  }
}

void DeckGUI::timerCallback()
{
    if (loopButton.getToggleState()) {
        if (player->getPositionRelative() > 1) {
            player->setPositionRelative(0);
            player->start();
        }
    }
    else
    {
        if (player->getPositionRelative() > 1) {
            player->setPositionRelative(0);
            posSlider.setValue(0);
            player->stop();
            playButton.setButtonText("Play");
        }
    }
    
    if (player->isLoaded()) {
        double position = player->getCurrentPosition();
        double totalLength = player->getTotalLength();

        posSlider.setValue(position / totalLength);

        String currentPositionString = formatTime(position, 2);

        currentTimeLabel.setText(currentPositionString, dontSendNotification);
    }
    waveformDisplay.setPositionRelative(player->getPositionRelative());
}

String DeckGUI::formatTime(double seconds, int decimalPlaces)
{
    int totalSeconds = (int)seconds;
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int secs = totalSeconds % 60;
    String timeString;
    if (hours > 0) {
        timeString = String::formatted("%d:%02d:%02d", hours, minutes, secs);
    }
    else {
        timeString = String::formatted("%d:%02d", minutes, secs);
    }
    return timeString;
}