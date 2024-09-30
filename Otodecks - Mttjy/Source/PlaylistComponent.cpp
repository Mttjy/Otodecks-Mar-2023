/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 20 Feb 2023 4:16:06pm
    Author:  matth

  ==============================================================================
*/

#include "PlaylistComponent.h"
#include <iostream>
#include <fstream>

//==============================================================================
PlaylistComponent::PlaylistComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    File urlFile = File::getCurrentWorkingDirectory().getChildFile("current_url.txt");
    urlFile.deleteFile();

    tableComponent.getHeader().addColumn("Track title", 1, 200);
    tableComponent.getHeader().addColumn("Duration", 2, 200);

    tableComponent.setModel(this);
    updateTrackTitles();
    loadTracks();
    addAndMakeVisible(tableComponent);

    addAndMakeVisible(deleteButton);
    deleteButton.setColour(deleteButton.buttonColourId, Colour::fromRGB(180, 10, 10));
    deleteButton.setButtonText("Delete");
    deleteButton.onClick = [this] { deleteSelectedTrack(); };

    searchBox.setTextToShowWhenEmpty("Search for tracks...", Colours::lightgrey);
    searchBox.setFont(18.0f);
    searchBox.onTextChange = [this] { updateTrackTitles(); };
    addAndMakeVisible(searchBox);
}

PlaylistComponent::~PlaylistComponent()
{
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void PlaylistComponent::resized()
{
    deleteButton.setBounds((getWidth() / 6) * 5, 0, (getWidth() / 6) * 1, 35);
    searchBox.setBounds(0, 0, (getWidth()/6)*5, 35);
    int tableWidth = getWidth();
    int columnWidth = tableWidth / 2;
    tableComponent.getHeader().setColumnWidth(1, columnWidth);
    tableComponent.getHeader().setColumnWidth(2, columnWidth);
    tableComponent.setBounds(0, 35, tableWidth, getHeight() - 35);
    tableComponent.getViewport()->setScrollBarsShown(true, false);
}

int PlaylistComponent::getNumRows()
{
    return trackTitles.size();
}

void PlaylistComponent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected && rowNumber != lastSelectedRow) // check if row changed
    {
        g.fillAll(Colour::fromRGB(200, 135, 220));
        File selectedFile = File::getCurrentWorkingDirectory().getChildFile("tracks").getChildFile(trackTitles[rowNumber]);
        currentURL = selectedFile.getFullPathName().toStdString()+".mp3";
        File urlFile = File::getCurrentWorkingDirectory().getChildFile("current_url.txt");
        urlFile.deleteFile();
        if (!urlFile.exists())
        {
            urlFile.create();
        }
        FileOutputStream outputStream(urlFile);
        outputStream << currentURL;
        outputStream.flush();
        lastSelectedRow = rowNumber;
    }
    else if (!rowIsSelected)
    {
        g.fillAll(Colour::fromRGB(200, 200, 200));
    }
    else if (rowIsSelected && rowNumber == lastSelectedRow)
    {
        g.fillAll(Colour::fromRGB(200, 135, 220));
    }
}

void PlaylistComponent::paintCell(Graphics& g, int rowNumber, int columnID, int width, int height, bool rowIsSelected)
{
    if (columnID == 1)
    {
        g.drawText(trackTitles[rowNumber], 2, 0, width - 4, height, Justification::centredLeft, true);
    }
    else if (columnID == 2)
    {
        g.drawText(trackDurations[rowNumber], 2, 0, width - 4, height, Justification::centredLeft, true);
    }
}

Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate)
{
    return existingComponentToUpdate;
}

void PlaylistComponent::updateTrackTitles()
{
    trackTitles.clear();
    trackDurations.clear();

    // Get a list of all the files in the "tracks" directory
    File tracksDir = File::getCurrentWorkingDirectory().getChildFile("tracks");
    Array<File> files = tracksDir.findChildFiles(File::TypesOfFileToFind::findFiles, false);

    // Loop through the files and add their paths to the trackTitles vector if they match the search query
    for (int i = 0; i < files.size(); ++i)
    {
        std::string filePath = files[i].getFullPathName().toStdString();
        std::string trackTitle = files[i].getFileNameWithoutExtension().toStdString();
        if (searchBox.getText().isEmpty() || trackTitle.find(searchBox.getText().toStdString()) != std::string::npos)
        {
            trackTitles.push_back(trackTitle);

            AudioFormatManager formatManager;
            formatManager.registerBasicFormats();
            std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(files[i]));

            if (reader != nullptr)
            {
                double durationInSeconds = reader->lengthInSamples / (double)reader->sampleRate;
                int minutes = (int)durationInSeconds / 60;
                int seconds = (int)durationInSeconds % 60;
                std::stringstream ss;
                ss << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;
                trackDurations.push_back(ss.str());
            }
            else
            {
                trackDurations.push_back("");
            }
        }
    }
    tableComponent.updateContent();
}


void PlaylistComponent::loadTracks()
{
    File tracksFolder = File::getCurrentWorkingDirectory().getChildFile("tracks");
    if (!tracksFolder.exists())
    {
        std::cerr << "Error: tracks folder does not exist" << std::endl;
        return;
    }

    trackTitles.clear();
    DirectoryIterator iter(tracksFolder, false, "*", File::TypesOfFileToFind::findFiles);
    while (iter.next())
    {
        trackTitles.push_back(iter.getFile().getFileNameWithoutExtension().toStdString());
    }

    tableComponent.updateContent();
}

void PlaylistComponent::writeStringToFile(const String& text, const File& file)
{
    FileOutputStream outputStream(file);
    if (outputStream.openedOk())
    {
        const char* data = text.toRawUTF8();
        const int dataSize = strlen(data);
        outputStream.write(data, dataSize);
    }
}

void PlaylistComponent::deleteSelectedTrack()
{
    int selectedRow = tableComponent.getSelectedRow();

    if (selectedRow < 0 || selectedRow >= trackTitles.size())
    {
        // show an error window if no file is selected
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
            "Error", "Please select a track to delete.");
        return;
    }

    // ask for confirmation
    int result = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
        "Delete Track", "Are you sure you want to delete " + trackTitles[selectedRow] + ".mp3 ?");

    if (result == 1)
    {
        File tracksDir = File::getCurrentWorkingDirectory().getChildFile("tracks");
        File trackFile = tracksDir.getChildFile(trackTitles[selectedRow] + ".mp3");
        trackFile.deleteFile();

        trackTitles.erase(trackTitles.begin() + selectedRow);
        trackDurations.erase(trackDurations.begin() + selectedRow);

        tableComponent.updateContent();
        tableComponent.deselectAllRows();
    }
    else // user clicked "Cancel" or closed the dialog
    {
        return;
        tableComponent.updateContent();
        tableComponent.deselectAllRows();
    }

}


