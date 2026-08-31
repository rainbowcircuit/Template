#include "Parameters.h"

Parameters::Parameters(TemplateAudioProcessor& p) : audioProcessor(p),
apvts(audioProcessor, nullptr, "Parameters", createParameterLayout())
{
#define X(id, ...) id = std::make_unique<ParameterInstance>(audioProcessor, *this, #id);
    TEMPLATE_ALL_SCALAR_PARAMS(X)
#undef X
}

void Parameters::registerInstance(ParameterInstance* instance)
{
    if (instance != nullptr)
        instances.push_back(instance);
}

void Parameters::prepare(double sampleRate, int samplesPerBlock)
{
    for (auto* instance : instances)
        instance->prepare(sampleRate, samplesPerBlock);
}

juce::AudioProcessorValueTreeState::ParameterLayout
Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

#define X(id, displayName, minVal, maxVal, step, skew, def) \
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{#id, 1}, \
                                                             displayName, \
                                                             juce::NormalisableRange<float>{minVal, maxVal, step, skew}, def));
    TEMPLATE_FLOAT_PARAMS(X)
#undef X

#define X(id, displayName, def) \
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{#id, 1}, displayName, def));
    TEMPLATE_BOOL_PARAMS(X)
#undef X

#define X(id, displayName, minVal, maxVal, def) \
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{#id, 1}, displayName, minVal, maxVal, def));
    TEMPLATE_INT_PARAMS(X)
#undef X

#define X(id, displayName, choices, defaultIndex) \
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{#id, 1}, displayName, choices, defaultIndex));
    TEMPLATE_CHOICE_PARAMS(X)
#undef X

    return layout;
}

ParameterInstance::ParameterInstance(TemplateAudioProcessor& p, Parameters& pm, juce::String paramID) : param(pm)
{
    this->paramID = paramID;

    auto* rawValue = param.apvts.getRawParameterValue(paramID);
    jassert(rawValue != nullptr);

    if (rawValue == nullptr)
        return;

    float initValue = rawValue->load();
    value.store(initValue);
    valueSafe = initValue;
    cachedValue.store(initValue);
    smoothed.reset(10);

    if (auto* parameter = dynamic_cast<juce::AudioProcessorParameterWithID*>(param.apvts.getParameter(paramID)))
    {
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(parameter))
        {
            rangedParam = ranged;
            rangedParam->addListener(this);
        }
    }

    param.registerInstance(this);
}

void ParameterInstance::prepare(double sampleRate, int samplesPerBlock) noexcept
{
    const auto blocksPerRamp = (int) std::ceil((smoothingTimeSeconds * sampleRate)
                                               / (double) juce::jmax(1, samplesPerBlock));

    smoothed.reset(juce::jmax(1, blocksPerRamp));
    smoothed.setCurrentAndTargetValue(get());
}

void ParameterInstance::parameterValueChanged (int /*maybe unused*/, float newValue)
{
    // load atomics for thread safe reading
    cachedValue.store(newValue);
    triggerUpdate();
    triggerAsyncUpdate();
}

void ParameterInstance::handleAsyncUpdate()
{
    if (rangedParam == nullptr)
        return;

    valueSafe = rangedParam->convertFrom0to1(cachedValue.load(std::memory_order_relaxed));
}

void ParameterInstance::triggerUpdate()
{
    if (rangedParam == nullptr)
        return;

    value.store(rangedParam->convertFrom0to1(cachedValue.load(std::memory_order_relaxed)));
}

float ParameterInstance::get() const noexcept
{
    return value.load(std::memory_order_relaxed);
}

float ParameterInstance::getSafe() const noexcept
{
    return valueSafe;
}

float ParameterInstance::getSmooth() noexcept
{
    smoothed.setTargetValue(get());
    return smoothed.getNextValue();
}

juce::RangedAudioParameter *ParameterInstance::getRangedAudioParameter() const noexcept
{
    return rangedParam;
}
