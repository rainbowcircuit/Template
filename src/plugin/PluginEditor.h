#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "../params/ParameterList.h"
#include <juce_gui_extra/juce_gui_extra.h>
//==============================================================================

class TemplateAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TemplateAudioProcessorEditor (TemplateAudioProcessor&);
    ~TemplateAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    TemplateAudioProcessor& audioProcessor;

#define X(id, ...) juce::WebSliderRelay id##Relay{#id};
    TEMPLATE_ALL_SCALAR_PARAMS(X)
#undef X

    static bool isWebView2RuntimeAvailable();
    juce::WebBrowserComponent::Options buildWebviewOptions();

    std::unique_ptr<juce::WebBrowserComponent> webview;
    juce::Label webViewUnavailableLabel;
    juce::HyperlinkButton webViewDownloadLink{
        "Download WebView2 Runtime",
        juce::URL("https://go.microsoft.com/fwlink/p/?LinkId=2124703")};

#define X(id, ...) WebSliderParameterAttachment id##Attachment { *audioProcessor.params->id->getRangedAudioParameter(), id##Relay, nullptr };
    TEMPLATE_ALL_SCALAR_PARAMS(X)
#undef X

    auto getResource(const juce::String& url) -> std::optional<juce::WebBrowserComponent::Resource>;
    std::unique_ptr<juce::ResizableBorderComponent> resizer;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TemplateAudioProcessorEditor)
};
