# Nebula - Implementation Plan

**Date:** 2026-04-24
**Complexity Score:** 4.6 (Complex)
**Strategy:** Phase-based implementation

---

## Complexity Factors

- **Parameters:** 3 parameters (3/5 = 0.6, capped at 2.0) = **0.6**
- **Algorithms:** 4 DSP components = **4**
  - Reverb Engine (`juce::dsp::Reverb`)
  - Mistero Lowpass Filter (`juce::dsp::ProcessorDuplicator<IIR::Filter>`)
  - Noise Generator (custom pink noise)
  - Dry/Wet Mixer (`juce::dsp::DryWetMixer`)
- **Features:** 0 points
  - No feedback loops (reverb internal comb loops are inside juce::dsp::Reverb — not custom)
  - No FFT/frequency-domain processing
  - No multiband processing
  - No modulation LFO or envelope
  - No external MIDI control
- **Total:** 0.6 + 4 + 0 = **4.6** (capped at 5.0)

**Note on pragmatic complexity:** Although the score is 4.6, Nebula is architecturally a low-risk plugin — all DSP components use well-established JUCE classes or trivially simple custom code. The phased breakdown below reflects this by keeping phases compact.

---

## Stages

- Stage 0: Research & Planning - COMPLETE
- Stage 1: Foundation (project structure + CMakeLists)
- Stage 2: Shell (APVTS parameters + compiling skeleton)
- Stage 3: DSP — Phase 4.1: Core Reverb + Dry/Wet
- Stage 3: DSP — Phase 4.2: Mistero Filter + Noise
- Stage 3: GUI — Phase 5.1: Layout + Basic Controls
- Stage 3: GUI — Phase 5.2: Parameter Binding + Interaction
- Stage 3: Validation (presets, pluginval, changelog)

---

## Complex Implementation (Score ≥ 3.0)

### Stage 3: DSP Phases

#### Phase 4.1: Core Reverb + Dry/Wet Mixer

**Goal:** Establish the primary audio path: dry signal capture → reverb → wet/dry blend. This produces a functional reverb plugin before any Nebula-specific features are added.

**Components:**
- `juce::dsp::Reverb` initialized with `prepare(spec)` and parameter set
- `juce::dsp::DryWetMixer<float>` with linear mixing rule
- `size` parameter wired to `reverbParams.roomSize`
- `mix` parameter wired to `setWetMixProportion()`
- Fixed `damping = 0.4`, `wetLevel = 1.0`, `dryLevel = 0.0`, `width = 1.0`
- Bus config: stereo in / stereo out, effect (not synth)
- Critical pattern #17: `prepare(spec)` + `process(context)` — NOT old API
- Critical pattern #4: BusesProperties in constructor, NOT prepareToPlay

**Test Criteria:**
- [ ] Plugin loads in DAW (VST3 + AU) without crashes
- [ ] Audio passes through when mix=0 (dry only — silence test)
- [ ] Reverb audible when mix > 0
- [ ] `size` parameter changes reverb character (small vs. large space perceptible)
- [ ] `mix` parameter smoothly blends dry and wet (0% = fully dry, 100% = fully wet)
- [ ] No clicks, dropouts, or denormal-related artifacts

---

#### Phase 4.2: Mistero Filter + Pink Noise Generator

**Goal:** Add the two defining Nebula-specific features to the wet path: the Mistero darkening filter and the analog hiss generator.

**Components:**
- `juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>` for Mistero
- `juce::SmoothedValue<float>` for misteroHz (20ms ramp, prevents zipper noise)
- `juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, hz, 0.707f)` coefficient update per block
- Pink noise generator: custom Voss–McCartney 6-stage implementation using `juce::Random`
- Noise amplitude constant: `kNoiseGain = 0.004f` (tune during validation)
- Noise scaled by `mix` parameter so noise=0 when mix=0
- Noise injected after Mistero filter (post-filter, not pre-filter — see architecture decision)
- Processing order enforced: reverb → Mistero filter → noise injection → DryWetMixer blend

**Test Criteria:**
- [ ] `mistero` parameter at 8000 Hz: reverb sounds relatively open (minimal filtering)
- [ ] `mistero` parameter at 1000 Hz: reverb tail clearly darkened/smothered
- [ ] `mistero` parameter at 200 Hz: extreme darkness — tail nearly sub-bass only
- [ ] Mistero filter does NOT affect the dry signal (guitar stays bright at any Mistero setting)
- [ ] Pink noise audible in the reverb tail at default settings (mix=0.4, size=0.5, mistero=6000Hz)
- [ ] Pink noise is NOT audible when mix=0 (silence → silence)
- [ ] No zipper noise when sweeping `mistero` knob quickly
- [ ] No artifacts or discontinuities when changing any parameter
- [ ] `juce::ScopedNoDenormals` active in processBlock()

---

### Stage 3: GUI Phases

#### Phase 5.1: Layout and Basic Controls

**Goal:** Integrate the UI mockup (when available) or build the three-knob layout. Establish the WebView shell with the dark/nebulous visual style.

**Components:**
- Copy UI mockup HTML to `Source/ui/public/index.html`
- Update `PluginEditor.h` / `PluginEditor.cpp` with WebView setup (critical patterns #3, #9, #11, #13)
- Configure `CMakeLists.txt`: `NEEDS_WEB_BROWSER TRUE`, `juce_gui_extra`, `juce_add_binary_data`
- Three knob controls: Mistero (center, larger), Size (left), Mix (right)
- Dark visual palette: navy, smoke grey, muted violet
- All controls visible at correct scale and position

**Test Criteria:**
- [ ] WebView window opens at correct size
- [ ] Three knobs visible with correct visual hierarchy (Mistero larger/centered)
- [ ] Dark color palette renders correctly (no white/bright backgrounds)
- [ ] Layout matches creative brief description (horizontal, centered)
- [ ] No WebView loading errors (check_native_interop.js served correctly — critical pattern #13)

---

#### Phase 5.2: Parameter Binding and Interaction

**Goal:** Full two-way parameter communication: knob drag → DSP update, DAW automation → knob visual update.

**Components:**
- Three `juce::WebSliderRelay` instances: misteroRelay, sizeRelay, mixRelay (critical pattern #11: unique_ptr, order: relays → webview → attachments)
- Three `juce::WebSliderParameterAttachment` instances with `nullptr` third argument (critical pattern #12)
- WebView `Options{}` chained `.withOptionsFrom()` for all three relays
- JavaScript: `import { getSliderState } from './js/juce/index.js'` with `type="module"` (critical pattern #21)
- Relative drag interaction for all three knobs (critical pattern #16)
- `valueChangedEvent.addListener(() => { value = state.getNormalisedValue(); })` pattern (critical pattern #15)
- Knob label display showing formatted values (Mistero: Hz, Size: %, Mix: %)

**Test Criteria:**
- [ ] Dragging each knob updates the corresponding DSP parameter in real-time
- [ ] DAW automation moves knobs visually
- [ ] Preset load/save updates all three knob positions correctly
- [ ] Knob drag feels natural (relative drag, not absolute — critical pattern #16)
- [ ] Parameter values display correctly in labels
- [ ] No frozen knobs (check: type="module", nullptr third arg, correct relay names)
- [ ] Plugin save/load restores all three parameter values correctly

---

### Implementation Flow

- Stage 0: Research & Planning - COMPLETE
- Stage 1: Foundation - project structure + CMakeLists.txt skeleton
- Stage 2: Shell - APVTS with 3 parameters compiling and audible through
- Stage 3: DSP
  - Phase 4.1: Core reverb + dry/wet mixer
  - Phase 4.2: Mistero filter + pink noise
- Stage 3: GUI
  - Phase 5.1: Layout and basic controls
  - Phase 5.2: Parameter binding and interaction
- Stage 3: Validation - presets, pluginval, changelog

---

## Implementation Notes

### Thread Safety

- All parameter reads: `parameters.getRawParameterValue("x")->load()` (atomic, audio thread safe)
- `juce::SmoothedValue<float>` for misteroHz: audio thread only, no sharing
- `juce::Random` noise instance: audio thread only, no sharing
- No locks needed anywhere in processBlock()

### Performance

- `juce::dsp::Reverb`: dominant CPU cost (~5–15% single core at 48kHz stereo)
- Mistero IIR filter: ~1% CPU (single biquad per channel)
- Pink noise (Voss–McCartney 6-stage): ~1% CPU
- Dry/Wet Mixer: ~1% CPU
- **Total estimated: ~10–20% single core at 48kHz stereo**

### Latency

- Zero reported latency (`getLatencySamples()` returns 0)
- `juce::dsp::Reverb` has no pre-delay (reverb starts immediately)
- No host compensation needed

### Denormal Protection

- `juce::ScopedNoDenormals noDenormals;` at top of `processBlock()`
- `juce::dsp::Reverb` handles its own internal denormal protection
- Pink noise is always non-zero (PRNG output), preventing denormals in the wet path

### Known Challenges

- **Mistero zipper noise:** Coefficient updates on the IIR filter can cause audible clicks when the knob is swept quickly. Solution: `juce::SmoothedValue<float>` with 20ms ramp on the misteroHz value before using it to compute coefficients.
- **Noise amplitude calibration:** `kNoiseGain = 0.004f` (~-48 dBFS) is a starting point. Tune during Phase 4.2 validation by ear against the reference brief (audible but subtle, like tape hiss).
- **JUCE WebSliderParameterAttachment third argument:** MUST pass `nullptr` as third argument — silent failure without it (critical pattern #12).
- **WebView ES6 module loading:** All JUCE WebView JS must use `type="module"` and `import { getSliderState }` (critical pattern #21).

---

## References

- Creative brief: `plugins/Nebula/.ideas/creative-brief.md`
- Parameter spec: `plugins/Nebula/.ideas/parameter-spec-draft.md`
- DSP architecture: `plugins/Nebula/.ideas/architecture.md`
- UI mockup: `plugins/Nebula/.ideas/mockups/` (pending)

**Reference plugins:**
- `DriveVerb` — identical DSP pattern: `juce::dsp::Reverb` + `juce::dsp::DryWetMixer` + IIR filter, exact same APVTS/processBlock structure to follow
- `FlutterVerb` — reference for WebView GUI with three knobs and SmoothedValue usage
- `GainKnob` — reference for minimal WebView parameter binding implementation (knob interaction pattern)
