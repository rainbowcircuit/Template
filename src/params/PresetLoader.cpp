#include <JuceHeader.h>
#include "PresetLoader.h"

const juce::File PresetManager::defaultDirectory{juce::File::getSpecialLocation(
                                                     juce::File::SpecialLocationType::commonDocumentsDirectory)
                                                     .getChildFile(ProjectInfo::companyName)
                                                     .getChildFile(ProjectInfo::projectName)};

const juce::String PresetManager::extension{"preset"};
const juce::String PresetManager::presetNameProperty{"presetName"};

PresetManager::PresetManager(juce::AudioProcessorValueTreeState &apvts) : apvts(apvts)
{
    if (!defaultDirectory.exists())
    {
        const auto result = defaultDirectory.createDirectory();
        if (result.failed())
        {
            DBG("Unable to create preset directory");
        }
    }

    apvts.state.addListener(this);
    currentPreset.referTo(apvts.state.getPropertyAsValue(presetNameProperty, nullptr));
}

PresetManager::~PresetManager()
{
    apvts.state.removeListener(this);
}

void PresetManager::loadPreset(const juce::String &presetName)
{
    if (presetName.isEmpty())
        return;
    const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
    if (!presetFile.existsAsFile())
    {
        // preset doesn't exist
        jassertfalse;
        return;
    }

    juce::XmlDocument xmlDocument{presetFile};
    const auto valueTreeToLoad = juce::ValueTree::fromXml(*xmlDocument.getDocumentElement());

    apvts.replaceState(valueTreeToLoad);
    currentPreset.setValue(presetName);
}

void PresetManager::savePreset(const juce::String &presetName)
{
    if (presetName.isEmpty())
        return;

    currentPreset.setValue(presetName);
    const auto xml = apvts.copyState().createXml();
    const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
    if (!xml->writeTo(presetFile))
    {
        DBG("Could not create preset file: " + presetFile.getFullPathName());
        jassertfalse;
    }
}

void PresetManager::attemptSave()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Enter Preset Name",
        defaultDirectory,
        "*." + extension);

    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [this](const juce::FileChooser &chooser)
    {
        const auto resultFile = chooser.getResult();
        if (resultFile == juce::File{}) return;
        savePreset(resultFile.getFileNameWithoutExtension());
    });
}

int PresetManager::loadNextPreset()
{
    const auto allPresets = getAllPreset();
    if (allPresets.isEmpty())
        return -1;

    const auto currentIndex = allPresets.indexOf(currentPreset.toString());
    const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;
    loadPreset(allPresets.getReference(nextIndex));
    return nextIndex;
}

int PresetManager::loadPreviousPreset()
{
    const auto allPresets = getAllPreset();
    if (allPresets.isEmpty())
        return -1;

    const auto currentIndex = allPresets.indexOf(currentPreset.toString());
    const auto previousIndex = currentIndex - 1 < 0 ? allPresets.size() - 1 : currentIndex - 1;
    loadPreset(allPresets.getReference(previousIndex));
    return previousIndex;
}

juce::StringArray PresetManager::getAllPreset() const
{
    juce::StringArray presets;
    const auto fileArray = defaultDirectory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.preset");

    for (const auto &file : fileArray)
    {
        presets.add(file.getFileNameWithoutExtension());
    }
    presets.sort(true);
    return presets;
}

juce::var PresetManager::getAllPresetAsVar()
{
    juce::Array<juce::var> result;
    const auto allPresets = getAllPreset();
    const auto currentIndex = allPresets.indexOf(currentPreset.toString());

    for (const auto &s : allPresets)
        result.add(s);

    auto *obj = new juce::DynamicObject();
    obj->setProperty("presets", result);
    obj->setProperty("currentIndex", currentIndex);

    return juce::var(obj);
}

juce::String PresetManager::getCurrentPreset() const
{
    return currentPreset.toString();
}


void PresetManager::valueTreeRedirected(juce::ValueTree &treeWhichHasBeenChanged)
{
    currentPreset.referTo(treeWhichHasBeenChanged.getPropertyAsValue(presetNameProperty, nullptr));
}
