# Parameter Specification (Draft)

**Status:** Draft - Awaiting UI mockup for full specification
**Created:** 2026-04-24
**Source:** Quick capture during ideation

This is a lightweight specification to enable parallel DSP research.
Full specification will be generated from finalized UI mockup.

## Parameters

### mistero
- **Type:** Float
- **Range:** 200 to 8000 Hz
- **Default:** 6000 Hz
- **DSP Purpose:** Frequenza di taglio del filtro passa-basso applicato esclusivamente alla coda wet del riverbero. Ruotando verso sinistra il suono si oscura progressivamente fino a un buio soffocato.

### size
- **Type:** Float
- **Range:** 0.0 to 1.0 (normalizzato)
- **Default:** 0.5
- **DSP Purpose:** Dimensione dello spazio virtuale. Controlla la densità della rete di comb/allpass filter e la lunghezza della coda del riverbero.

### mix
- **Type:** Float
- **Range:** 0.0 to 1.0 (0% – 100%)
- **Default:** 0.4 (40% wet)
- **DSP Purpose:** Bilanciamento lineare tra segnale originale (dry) e segnale processato (wet). Il noise/hiss è parte del segnale wet.

## Next Steps

- [ ] Complete UI mockup workflow (`/dream Nebula` → opzione mockup)
- [ ] Finalize design and generate full parameter-spec.md
- [ ] Validate consistency between draft and final spec
