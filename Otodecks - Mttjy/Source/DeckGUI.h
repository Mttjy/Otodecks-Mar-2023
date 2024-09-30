/*
  ==============================================================================

    DeckGUI.h
    Created: 13 Mar 2020 6:44:48pm
    Author:  matthew

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "PlaylistComponent.h"

//==============================================================================
/*
*/
class DeckGUI    : public Component,
                   public Button::Listener, 
                   public Slider::Listener, 
                   public FileDragAndDropTarget, 
                   public Timer
{
public:
    DeckGUI(DJAudioPlayer* player, 
           AudioFormatManager & 	formatManagerToUse,
           AudioThumbnailCache & 	cacheToUse,
           PlaylistComponent* playlistComponent);
    ~DeckGUI();

    void paint (Graphics&) override;
    void resized() override;

     /** implement Button::Listener */
    void buttonClicked (Button *button) override;

    /** implement Slider::Listener */
    void sliderValueChanged (Slider *slider) override;

    bool isInterestedInFileDrag (const StringArray &files) override;
    void filesDropped (const StringArray &files, int x, int y) override; 

    void timerCallback() override; 

private:
    String formatTime(double seconds, int decimalPlaces);
    String selectedURL;

    TextButton playButton{"PLAY"};
    TextButton stopButton{"STOP"};
    TextButton loadButton{"LOAD"};
    TextButton ffButton{ "FF" };
    TextButton resButton{ "Restart" };
    ToggleButton loopButton{ "Loop" };
  
    Slider volSlider; 
    Slider speedSlider;
    Slider posSlider;
    Label nowPlayingLabel;
    Label currentTimeLabel;
    Label totalTimeLabel;

    FileChooser fChooser{"Select a file..."};
    bool fileIsLoaded = false;


    WaveformDisplay waveformDisplay;

    DJAudioPlayer* player; 
    PlaylistComponent* _playlistComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
