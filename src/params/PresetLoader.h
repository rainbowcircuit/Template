#pragma once
#include <JuceHeader.h>

class PresetManager : juce::ValueTree::Listener
{
public:
    static const juce::File defaultDirectory;
    static const juce::String extension;
    static const juce::String presetNameProperty;

    PresetManager(juce::AudioProcessorValueTreeState &apvts);
    ~PresetManager();

    void loadPreset(const juce::String &presetName);
    void savePreset(const juce::String &presetName);
    void attemptSave();
    int loadNextPreset();
    int loadPreviousPreset();

    juce::StringArray getAllPreset() const;
    juce::var getAllPresetAsVar();

    juce::String getCurrentPreset() const;

private:
    void valueTreeRedirected(juce::ValueTree &treeWhichHasBeenChanged) override;
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::AudioProcessorValueTreeState &apvts;
    juce::Value currentPreset;
};
