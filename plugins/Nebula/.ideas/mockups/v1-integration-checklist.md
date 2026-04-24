# Stage 3 (GUI) Integration Checklist — Nebula v1

**Plugin:** Nebula
**Mockup Version:** v1
**Generated:** 2026-04-24
**Window:** 520 x 300 px, fixed (non-resizable)
**Parameters:** size (Float), mistero (Float), mix (Float)

---

## 1. Copy UI Files

- [ ] Copy `v1-ui.html` to `Source/ui/public/index.html`
- [ ] Copy JUCE frontend JS to `Source/ui/public/js/juce/index.js`
      (source: `plugins/DriveVerb/Source/ui/public/js/juce/index.js` or JUCE framework)
- [ ] Copy `check_native_interop.js` to `Source/ui/public/js/juce/check_native_interop.js`
      (source: `plugins/DriveVerb/Source/ui/public/js/juce/check_native_interop.js`)
- [ ] Verify directory structure:
      ```
      Source/ui/public/
      ├── index.html
      └── js/juce/
          ├── index.js
          └── check_native_interop.js
      ```

---

## 2. Update PluginEditor.h

- [ ] Replace `PluginEditor.h` content using `v1-PluginEditor-TEMPLATE.h` as reference
- [ ] Verify member declaration order (Pattern #11):
      1. `sizeRelay`, `misteroRelay`, `mixRelay` (relays first)
      2. `webView` (WebView second)
      3. `sizeAttachment`, `misteroAttachment`, `mixAttachment` (attachments last)
- [ ] Verify include: `#include <juce_gui_extra/juce_gui_extra.h>`
- [ ] Confirm class name matches actual processor class name

---

## 3. Update PluginEditor.cpp

- [ ] Replace `PluginEditor.cpp` content using `v1-PluginEditor-TEMPLATE.cpp` as reference
- [ ] Verify constructor initialization order matches declaration order
- [ ] Confirm relay IDs match APVTS parameter IDs exactly (case-sensitive):
      - `"size"` — matches APVTS `addParameter(..."size"...)`
      - `"mistero"` — matches APVTS `addParameter(..."mistero"...)`
      - `"mix"` — matches APVTS `addParameter(..."mix"...)`
- [ ] Verify `WebSliderParameterAttachment` uses 3 arguments including `nullptr` (Pattern #12)
- [ ] Verify `setSize(520, 300)` and `setResizable(false, false)`
- [ ] Verify `getResource()` serves all three URLs:
      - `"/"` and `"/index.html"` → `BinaryData::index_html`
      - `"/js/juce/index.js"` → `BinaryData::index_js`
      - `"/js/juce/check_native_interop.js"` → `BinaryData::check_native_interop_js`
- [ ] Verify MIME types use `"application/javascript"` for .js files (not `"text/javascript"`)

---

## 4. Update CMakeLists.txt

- [ ] Add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin()` call (Pattern #9)
- [ ] Add `juce_add_binary_data(Nebula_UIResources ...)` block with all 3 UI files
- [ ] Add `juce::juce_gui_extra` to `target_link_libraries` PRIVATE section (Pattern #3)
- [ ] Add `Nebula_UIResources` to `target_link_libraries` PRIVATE section
- [ ] Verify `juce_generate_juce_header(Nebula)` comes AFTER `target_link_libraries` (Pattern #1)
- [ ] Add compile definitions: `JUCE_WEB_BROWSER=1`, `JUCE_USE_CURL=0` (Pattern #3)
- [ ] Use `v1-CMakeLists-SNIPPET.txt` as complete reference

---

## 5. Build — Debug Configuration

- [ ] Configure CMake: `cmake -B build -DCMAKE_BUILD_TYPE=Debug`
- [ ] Build Standalone target: `cmake --build build --target Nebula_Standalone`
- [ ] Build succeeds with zero errors and zero warnings (or only expected warnings)
- [ ] Run Standalone

---

## 6. Verify WebView Loads (Debug)

- [ ] Standalone opens and shows UI (not blank white window)
- [ ] Right-click → "Inspect Element" opens DevTools (macOS/Linux WebKit)
- [ ] DevTools Console shows no JavaScript errors
- [ ] `window.__JUCE__` object exists in DevTools console
- [ ] `typeof getSliderState` returns `'function'` (ES6 module loaded)
- [ ] All 3 knobs visible and render correctly

---

## 7. Test Parameter Binding

- [ ] Dragging **Size** knob moves knob visually and updates "50%" value display
- [ ] Dragging **Mistero** knob moves knob and shows Hz/kHz value (logarithmic)
- [ ] Dragging **Mix** knob moves knob and updates percentage display
- [ ] Double-clicking any knob resets to default (Size: 50%, Mistero: 6.0 kHz, Mix: 40%)
- [ ] DAW automation lane controls knob position in UI
- [ ] Preset recall updates all 3 knobs correctly
- [ ] Values persist after closing and reopening plugin window

---

## 8. Build — Release Configuration

- [ ] Build Release: `cmake --build build --config Release --target Nebula_Standalone`
- [ ] Release build succeeds
- [ ] Install plugin: run `./scripts/build-and-install.sh Nebula` (Pattern #10)
- [ ] Reload plugin 10 times in DAW — zero crashes (validates member order, Pattern #11)
- [ ] Scan for zombie processes after each reload

---

## 9. VST3 / AU Validation

- [ ] VST3 appears in DAW scanner after install
- [ ] AU component validates: `auval -v aufx Nbla Manu`
- [ ] Plugin shows correct category (Audio Effect, not Instrument)
- [ ] All 3 parameters visible in DAW automation list

---

## 10. WebView-Specific Validation

- [ ] No viewport units in CSS (`100vh`, `100vw` — search `index.html`)
- [ ] `user-select: none` present in CSS (native application feel)
- [ ] Context menu disabled (right-click inside UI shows nothing)
- [ ] No 404 responses in resource provider (check console log)
- [ ] Correct MIME types: `.js` files served as `application/javascript`
- [ ] `check_native_interop.js` served successfully (Pattern #13)

---

## Parameter Reference (from parameter-spec.md)

| ID       | Type  | Range          | Default | Scale       | Relay Type             |
|----------|-------|----------------|---------|-------------|------------------------|
| size     | Float | 0.0 – 1.0      | 0.5     | Linear      | WebSliderRelay         |
| mistero  | Float | 200 – 8000 Hz  | 6000    | Logarithmic | WebSliderRelay         |
| mix      | Float | 0.0 – 1.0      | 0.4     | Linear      | WebSliderRelay         |

All 3 parameters use `WebSliderParameterAttachment` with `nullptr` as third argument (Pattern #12).

---

## Notes for gui-agent

- **Mistero is logarithmic in the UI only.** The C++ APVTS parameter is registered as a
  standard `AudioParameterFloat` with a log skew factor. The JavaScript side handles the
  log/linear conversion independently. No special C++ handling is needed for the JUCE
  relay — the normalised value (0.0–1.0) is communicated and JS maps it to Hz.
- **No VU meter or timer** in this plugin. `timerCallback` is not needed.
- **Reference plugin:** DriveVerb uses the identical WebView pattern (size, decay, dryWet,
  drive, filter). Use it as a working model when debugging.
- **Critical patterns file:** `troubleshooting/patterns/juce8-critical-patterns.md`
