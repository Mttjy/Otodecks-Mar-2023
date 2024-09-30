/*
  ==============================================================================

    PlaylistComponent.h
    Created: 20 Feb 2023 4:16:06pm
    Author:  matth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>


//==============================================================================
/*
*/
class PlaylistComponent  : public Component, public TableListBoxModel
{
public:
    PlaylistComponent();
    ~PlaylistComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    int getNumRows() override;

    void paintRowBackground(Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;

    void paintCell(Graphics&, int rowNumber, int columnID, int width, int height, bool rowIsSelected) override;

    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

    void writeStringToFile(const String& text, const File& file);
    void loadTracks();
    void updateTrackTitles();
    void deleteSelectedTrack();

private:
    std::string searchQuery;
    juce::TextEditor searchBox;
    TableListBox tableComponent;
    std::vector<std::string> trackTitles;
    std::vector<std::string> trackDurations;
    String currentURL;
    int lastSelectedRow = -1;
    TextButton deleteButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};




