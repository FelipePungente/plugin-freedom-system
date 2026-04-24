#pragma once

// ============================================================================
// v1-PluginEditor-TEMPLATE.h
//
// TEMPLATE FILE — for gui-agent reference during Stage 3 (GUI implementation).
// Do NOT copy-paste directly. gui-agent adapts this to the actual plugin
// structure, existing includes, and PluginProcessor API.
//
// Generated: 2026-04-24
// Plugin:    Nebula
// Mockup:    v1
// Parameters: mistero (Float), size (Float), mix (Float)
// ============================================================================

#include "PluginProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h>   // Required: WebBrowserComponent (Pattern #3)

class NebulaAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit NebulaAudioProcessorEditor(NebulaAudioProcessor&);
    ~NebulaAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    NebulaAudioProcessor& processorRef;

    // ========================================================================
    // CRITICAL MEMBER DECLARATION ORDER (Pattern #11)
    //
    // C++ members are destroyed in REVERSE order of declaration.
    // WebSliderParameterAttachment calls evaluateJavascript() on destruction,
    // so it MUST be destroyed BEFORE webView. Declare attachments LAST.
    //
    // Order: Relays -> WebView -> Attachments
    // ========================================================================

    // 1. RELAYS FIRST (no dependencies, destroyed last)
    std::unique_ptr<juce::WebSliderRelay> sizeRelay;
    std::unique_ptr<juce::WebSliderRelay> misteroRelay;
    std::unique_ptr<juce::WebSliderRelay> mixRelay;

    // 2. WEBVIEW SECOND (depends on relays via withOptionsFrom)
    std::unique_ptr<juce::WebBrowserComponent> webView;

    // 3. ATTACHMENTS LAST (depend on relays AND webView, destroyed first)
    std::unique_ptr<juce::WebSliderParameterAttachment> sizeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> misteroAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> mixAttachment;

    // Resource provider for binary HTML/JS data (Pattern #8)
    std::optional<juce::WebBrowserComponent::Resource> getResource(const juce::String& url);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NebulaAudioProcessorEditor)
};
