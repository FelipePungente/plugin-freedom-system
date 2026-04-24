// ============================================================================
// v1-PluginEditor-TEMPLATE.cpp
//
// TEMPLATE FILE — for gui-agent reference during Stage 3 (GUI implementation).
// Do NOT copy-paste directly. gui-agent adapts this to the actual plugin
// structure (PluginProcessor API, existing APVTS parameter IDs, etc.).
//
// Generated: 2026-04-24
// Plugin:    Nebula
// Mockup:    v1
// Window:    520 x 300 px (fixed, non-resizable)
// Parameters: mistero (Float), size (Float), mix (Float)
//
// Critical patterns applied:
//   #8  - Explicit resource URL mapping
//   #11 - Member order: relays -> webView -> attachments
//   #12 - WebSliderParameterAttachment requires 3 args (nullptr for undoManager)
//   #13 - check_native_interop.js served from resource provider
//   #21 - ES6 module loading (type="module" in HTML)
// ============================================================================

#include "PluginEditor.h"
#include "BinaryData.h"

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────
NebulaAudioProcessorEditor::NebulaAudioProcessorEditor(NebulaAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // ── 1. Create relays FIRST (Pattern #11) ─────────────────────────────────
    // Parameter IDs must exactly match those registered in PluginProcessor APVTS.
    sizeRelay    = std::make_unique<juce::WebSliderRelay>("size");
    misteroRelay = std::make_unique<juce::WebSliderRelay>("mistero");
    mixRelay     = std::make_unique<juce::WebSliderRelay>("mix");

    // ── 2. Create WebView with all relays registered (Pattern #8, #11) ───────
    webView = std::make_unique<juce::WebBrowserComponent>(
        juce::WebBrowserComponent::Options{}
            .withNativeIntegrationEnabled()
            .withResourceProvider([this](auto& url) { return getResource(url); })
            .withOptionsFrom(*sizeRelay)
            .withOptionsFrom(*misteroRelay)
            .withOptionsFrom(*mixRelay)
    );

    // ── 3. Create attachments LAST (Patterns #11, #12) ───────────────────────
    // Three parameters required in JUCE 8: (parameter, relay, undoManager).
    // Pass nullptr for undoManager unless the plugin uses one.
    sizeAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("size"),
        *sizeRelay,
        nullptr
    );

    misteroAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("mistero"),
        *misteroRelay,
        nullptr
    );

    mixAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("mix"),
        *mixRelay,
        nullptr
    );

    // ── 4. Add WebView and navigate to UI ────────────────────────────────────
    addAndMakeVisible(*webView);
    webView->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());

    // ── 5. Fixed window size (from YAML: 520 x 300, non-resizable) ───────────
    setResizable(false, false);
    setSize(520, 300);
}

// ─────────────────────────────────────────────────────────────────────────────
// Destructor
// ─────────────────────────────────────────────────────────────────────────────
NebulaAudioProcessorEditor::~NebulaAudioProcessorEditor()
{
    // Destruction order is automatic (reverse of declaration):
    //   attachments destroyed first -> webView second -> relays last
    // This is correct. Do NOT manually reset members here.
}

// ─────────────────────────────────────────────────────────────────────────────
// paint — WebView handles all visual rendering
// ─────────────────────────────────────────────────────────────────────────────
void NebulaAudioProcessorEditor::paint(juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

// ─────────────────────────────────────────────────────────────────────────────
// resized — WebView fills the entire editor
// ─────────────────────────────────────────────────────────────────────────────
void NebulaAudioProcessorEditor::resized()
{
    webView->setBounds(getLocalBounds());
}

// ─────────────────────────────────────────────────────────────────────────────
// getResource — Explicit URL mapping for BinaryData (Pattern #8)
//
// BinaryData converts file paths to C++ identifiers:
//   Source/ui/public/index.html               -> BinaryData::index_html
//   Source/ui/public/js/juce/index.js         -> BinaryData::index_js
//   Source/ui/public/js/juce/check_native_interop.js -> BinaryData::check_native_interop_js
//
// Each URL the HTML references must have an explicit mapping here.
// ─────────────────────────────────────────────────────────────────────────────
std::optional<juce::WebBrowserComponent::Resource>
NebulaAudioProcessorEditor::getResource(const juce::String& url)
{
    // Helper: convert BinaryData char array to std::vector<std::byte>
    auto makeVector = [](const char* data, int size) {
        return std::vector<std::byte>(
            reinterpret_cast<const std::byte*>(data),
            reinterpret_cast<const std::byte*>(data) + size
        );
    };

    // Root / index.html
    if (url == "/" || url == "/index.html")
    {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_html, BinaryData::index_htmlSize),
            juce::String("text/html")
        };
    }

    // JUCE WebView bridge (ES6 module - Pattern #21)
    if (url == "/js/juce/index.js")
    {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_js, BinaryData::index_jsSize),
            juce::String("application/javascript")
        };
    }

    // Native interop verification script (Pattern #13 - REQUIRED)
    if (url == "/js/juce/check_native_interop.js")
    {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::check_native_interop_js,
                       BinaryData::check_native_interop_jsSize),
            juce::String("application/javascript")
        };
    }

    // Resource not found - log for debugging
    juce::Logger::writeToLog("NebulaEditor: resource not found: " + url);
    return std::nullopt;
}
