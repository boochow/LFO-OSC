#include "userosc.h"

typedef struct State {
    float phase;
    float phase_h;
    uint8_t flags;
    float shape;
    float shiftshape;
    uint8_t waveform;
} State;

enum {
    k_flags_none = 0,
    k_flag_reset = 1<<0,
};

enum {
    SINE = 0,
    TRIANGULAR,
    PARABOLIC,
    SAWTOOTH,
    RAMPDOWN,
    SQUARE,
};

enum {
    WAVEFORM = 2,
};

static State s_state;

void OSC_INIT(uint32_t platform, uint32_t api)
{
    s_state.phase = 0.f;
    s_state.phase_h = 0.f;
    s_state.flags = k_flags_none;
}

void OSC_CYCLE(const user_osc_param_t * const params,
               int32_t *yn,
               const uint32_t frames)
{
    const uint8_t flags = s_state.flags;
    s_state.flags = k_flags_none;
    
    const float w0_h = osc_w0f_for_note((params->pitch)>>8, params->pitch & 0xFF);
    const float w0 = w0_h * expf((-2.f + s_state.shape + s_state.shape) * M_LN10);
    float phase = (flags & k_flag_reset) ? 0.f : s_state.phase;
    float phase_h = (flags & k_flag_reset) ? 0.f : s_state.phase_h;
  
    q31_t * __restrict y = (q31_t *)yn;
    const q31_t * y_e = y + frames;
  
    for (; y != y_e; ) {
        float sig;
        float sig_h;
        switch(s_state.waveform) {
        case SINE:
            sig = osc_sinf(phase);
            sig_h = osc_sinf(phase_h);
            break;
        case TRIANGULAR:
            sig = (phase - 0.5f <= 0.f) ? 2 * phase : 2 * (1 - phase);
            sig_h = (phase_h - 0.5f <= 0.f) ? 2 * phase_h : 2 * (1 - phase_h);
            break;
        case PARABOLIC:
            sig = osc_parf(phase);
            sig_h = osc_parf(phase_h);
            break;
        case SAWTOOTH:
            sig = osc_sawf(phase);
            sig_h = osc_sawf(phase_h);
            break;
        case RAMPDOWN:
            sig = osc_sawf(1.f - phase);
            sig_h = osc_sawf(1.f - phase_h);
            break;
        case SQUARE:
            sig = osc_sqrf(phase);
            sig_h = osc_sqrf(phase_h);
            break;
        default:
            sig = (phase - 0.5f <= 0.f) ? 1.f : -1.f;
            sig_h = (phase_h - 0.5f <= 0.f) ? 1.f : -1.f;
        }

        sig = (1. - s_state.shiftshape) * sig + s_state.shiftshape * sig_h;
        *(y++) = f32_to_q31(sig);

        phase += w0;
        phase -= (uint32_t)phase;
        phase_h += w0_h;
        phase_h -= (uint32_t)phase_h;
    }
    s_state.phase = phase;
    s_state.phase_h = phase_h;
}

void OSC_NOTEON(const user_osc_param_t * const params)
{
    s_state.flags |= k_flag_reset;
}

void OSC_NOTEOFF(const user_osc_param_t * const params)
{
    (void)params;
}

void OSC_PARAM(uint16_t index, uint16_t value)
{
    switch(index) {
    case k_user_osc_param_shape:
        s_state.shape = param_val_to_f32(value);
        break;
    case k_user_osc_param_shiftshape:
        s_state.shiftshape = param_val_to_f32(value); 
        break;
    case k_user_osc_param_id1:
        s_state.waveform = value;
        break;
    default:
        break;
    }
}
