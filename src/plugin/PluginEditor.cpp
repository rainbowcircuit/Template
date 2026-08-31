#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

#if JUCE_WINDOWS
 #include <WebView2.h>
#endif

//==============================================================================

namespace {
    static const char* getMimeForExtension(const juce::String& extension) {
        static const std::unordered_map<juce::String, const char*> mimeMap = {
            {{"htm"}, "text/html"},
            {{"html"}, "text/html"},
            {{"txt"}, "text/plain"},
            {{"jpg"}, "image/jpeg"},
            {{"jpeg"}, "image/jpeg"},
            {{"svg"}, "image/svg+xml"},
            {{"ico"}, "image/vnd.microsoft.icon"},
            {{"json"}, "application/json"},
            {{"png"}, "image/png"},
            {{"css"}, "text/css"},
            {{"map"}, "application/json"},
            {{"js"}, "text/javascript"},
            {{"woff2"}, "font/woff2"}};

        if (const auto it = mimeMap.find(extension.toLowerCase());
            it != mimeMap.end())
            return it->second;

        jassertfalse;
        return "";
        }
}


//==============================================================================
bool TemplateAudioProcessorEditor::isWebView2RuntimeAvailable()
{
#if JUCE_WINDOWS
    LPWSTR versionInfo = nullptr;
    const auto hr = GetAvailableCoreWebView2BrowserVersionString(nullptr, &versionInfo);
    const bool available = SUCCEEDED(hr) && versionInfo != nullptr;

    if (versionInfo != nullptr)
        CoTaskMemFree(versionInfo);

    return available;
#else
    return true;
#endif
}

juce::WebBrowserComponent::Options TemplateAudioProcessorEditor::buildWebviewOptions()
{
    auto options = juce::WebBrowserComponent::Options{};

#define X(id, ...) options = options.withOptionsFrom(id##Relay);
    TEMPLATE_ALL_SCALAR_PARAMS(X)
#undef X

    options = options.withNativeFunction("attemptSave", [this](auto &var, auto completion)
    {
        juce::MessageManager::callAsync([this] { audioProcessor.presets->attemptSave(); });
        completion(juce::var());
    });

    options = options.withNativeFunction("getAllPreset", [this](auto &var, auto completion)
    {
        completion(audioProcessor.presets->getAllPresetAsVar());
    });

    options = options.withNativeFunction("loadPreset", [this](auto &var, auto completion)
    {
        audioProcessor.presets->loadPreset(var[0].toString());
        completion(juce::var());
    });

    options = options.withResourceProvider([this](const auto &url)
                                           { return getResource(url); });

#if JUCE_WINDOWS
    options = options.withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
                     .withWinWebView2Options(
                         juce::WebBrowserComponent::Options::WinWebView2{}
                             .withUserDataFolder(juce::File::getSpecialLocation(
                                 juce::File::SpecialLocationType::tempDirectory)));
#endif

    return options;
}

TemplateAudioProcessorEditor::TemplateAudioProcessorEditor(TemplateAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    if (isWebView2RuntimeAvailable())
    {
        webview = std::make_unique<juce::WebBrowserComponent>(buildWebviewOptions());
        addAndMakeVisible(*webview);

    #if JUCE_DEBUG
        webview->goToURL("http://localhost:4000");
    #else
        webview->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
    #endif
    }
    else
    {
        webViewUnavailableLabel.setText(
            "This plugin's interface requires the Microsoft Edge WebView2 Runtime, "
            "which isn't installed on this computer.",
            juce::dontSendNotification);
        webViewUnavailableLabel.setJustificationType(juce::Justification::centred);
        webViewUnavailableLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(webViewUnavailableLabel);
        addAndMakeVisible(webViewDownloadLink);
    }

    const int width = 480;
    setResizable(true, false);
    setResizeLimits((int)width * 0.75f, (int)width * 0.75f / 2, (int)width, (int)width / 2);
    getConstrainer()->setFixedAspectRatio(2);

    resizer = std::make_unique<juce::ResizableBorderComponent>(this, getConstrainer()); // resizer created AFTER
    addAndMakeVisible(resizer.get());

    setSize((int)width * 0.875f, (int)width * 0.875f / 2);

#define X(id, ...) id##Attachment.sendInitialUpdate();
    TEMPLATE_ALL_SCALAR_PARAMS(X)
#undef X
}

TemplateAudioProcessorEditor::~TemplateAudioProcessorEditor()
{
    webview.reset();
}

//==============================================================================
void TemplateAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1d1d1d));
}

void TemplateAudioProcessorEditor::resized()
{
    const auto webviewBounds = getLocalBounds();

    if (webview != nullptr)
    {
        webview->setBounds(webviewBounds);

        auto *windowSize = new juce::DynamicObject();
        windowSize->setProperty("width", webviewBounds.getWidth());
        windowSize->setProperty("height", webviewBounds.getHeight());
        webview->emitEventIfBrowserIsVisible("windowSize", juce::JSON::toString(juce::var(windowSize)));
    }
    else
    {
        auto bounds = webviewBounds.reduced(20);
        webViewUnavailableLabel.setBounds(bounds.removeFromTop(bounds.getHeight() / 2));
        webViewDownloadLink.setBounds(bounds.removeFromTop(24));
    }

    if (resizer != nullptr)
        resizer->setBounds(getLocalBounds());
}

auto TemplateAudioProcessorEditor::getResource(const juce::String& url) -> std::optional<juce::WebBrowserComponent::Resource>
{
    const auto resourceToRetrieve = url == "/" ? juce::String("index.html") : url.fromFirstOccurrenceOf("/", false, false);

    for (int i = 0; i < TemplateUIData::namedResourceListSize; ++i)
    {
        const auto* symbolName = TemplateUIData::namedResourceList[i];
        const auto* originalFilename = TemplateUIData::getNamedResourceOriginalFilename(symbolName);

        if (originalFilename == nullptr || resourceToRetrieve != originalFilename)
            continue;

        int dataSizeInBytes = 0;
        const auto* data = TemplateUIData::getNamedResource(symbolName, dataSizeInBytes);

        if (data == nullptr)
            return std::nullopt;

        const auto* bytes = reinterpret_cast<const std::byte*>(data);
        const auto extension = resourceToRetrieve.fromLastOccurrenceOf(".", false, false);
        return juce::WebBrowserComponent::Resource{
            std::vector<std::byte>(bytes, bytes + dataSizeInBytes),
            getMimeForExtension(extension)};
    }

    return std::nullopt;
}
