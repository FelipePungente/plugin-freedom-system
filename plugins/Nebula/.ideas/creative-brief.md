# Nebula - Creative Brief

## Overview

**Type:** Audio Effect (Reverb)
**Core Concept:** Riverbero lo-fi con carattere nebbioso, arricchito da hiss/noise nella coda e oscurato da un filtro passa-basso chiamato "Mistero"
**Status:** 💡 Ideated
**Created:** 2026-04-24

## Vision

Nebula è un riverbero che non vuole essere pulito. La sua anima è nella nebbia: una coda sporca di hiss analogico, un filtro che spegne progressivamente le alte frequenze fino a soffocare il suono in un buio vellutato. Il knob "Mistero" è il cuore del plugin — più lo giri, più il riverbero si oscura, come ascoltare qualcosa da dietro una porta chiusa o dal fondo di un corridoio. Non è un riverbero da studio professionale: è lo spazio sonoro di una stanza abbandonata, di un nastro vissuto, di un ricordo sfocato.

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Mistero | 200 Hz – 8 kHz | 6 kHz | Frequenza di taglio del filtro passa-basso applicato alla coda del riverbero. Più basso = più scuro e soffocato |
| Size | Small – Large (0.0 – 1.0) | 0.5 | Dimensione dello spazio virtuale. Influenza la densità e la lunghezza della coda |
| Wet / Dry | 0% – 100% | 40% | Bilanciamento tra segnale originale e riverbero processato |

## UI Concept

**Layout:** Tre knob in orizzontale, centrati. "Mistero" al centro, leggermente più grande degli altri due.
**Visual Style:** Buio, nebbioso — palette di colori scuri (navy, grigio fumo, viola muted). Possibile particellare o texture granulare come sfondo animato.
**Key Elements:** Il knob "Mistero" come elemento visivo dominante

## Use Cases

- Voci intimate in stile bedroom pop / indie lo-fi
- Pad e synth atmosferici che necessitano di spazio nebbioso e onirico
- Chitarre acustiche o elettriche con riverbero vintage sporco
- Qualsiasi sorgente dove si vuole nascondere il suono nel buio

## Inspirations

- Valheim OST (atmosfere nebbiose e misteriose)
- Effetti nastro/tape degli anni '70-'80
- OTO Biscuit (bitcrushing/texture lo-fi)
- Valhalla Vintage Verb (carattere algoritmo vintage)

## Technical Notes

- Il filtro "Mistero" agisce **solo sulla coda del riverbero** (wet path), lasciando il segnale dry intatto
- Il noise/hiss è generato e miscelato nella coda del riverbero — non sul segnale dry
- Algoritmo di riverbero suggerito: Schroeder o Freeverb (comb + allpass filters) per carattere lo-fi naturale
- Il noise può essere modellato come white/pink noise con envelope seguendo la coda del riverbero

## Next Steps

- [ ] Create UI mockup (`/dream Nebula` → opzione mockup)
- [ ] Start implementation (`/implement Nebula`)
