#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TemplateAudioProcessor::TemplateAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    params = std::make_unique<Parameters>(*this);
    presets = std::make_unique<PresetManager>(params->apvts);
}


TemplateAudioProcessor::~TemplateAudioProcessor()
{
}

//==============================================================================
const juce::String TemplateAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TemplateAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TemplateAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TemplateAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TemplateAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TemplateAudioProcessor::getNumPrograms()
{
    return 1;
}

int TemplateAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TemplateAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TemplateAudioProcessor::getProgramName (int index)
{
    return {};
}

void TemplateAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TemplateAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    params->prepare(sampleRate, samplesPerBlock);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels = (juce::uint32) getTotalNumOutputChannels();
    gainProcessor.prepare(spec);
}

void TemplateAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TemplateAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TemplateAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i){
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    gainProcessor.setGainDecibels(params->gain->getSmooth());

    juce::dsp::AudioBlock<float> block(buffer);
    gainProcessor.process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
bool TemplateAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* TemplateAudioProcessor::createEditor()
{
    return new TemplateAudioProcessorEditor (*this);
}

//==============================================================================
void TemplateAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    copyXmlToBinary(*params->apvts.copyState().createXml(), destData);
}

void TemplateAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto xmlState = getXmlFromBinary(data, sizeInBytes);
    if (xmlState == nullptr)
        return;
    const auto newTree = juce::ValueTree::fromXml(*xmlState);
    params->apvts.replaceState(newTree);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TemplateAudioProcessor();
}
