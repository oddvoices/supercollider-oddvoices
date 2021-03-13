// PluginOddVoicesSynth.cpp
// Nathan Ho (nathan.waikin.ho@gmail.com)

#include "SC_PlugIn.hpp"
#include "OddVoicesSynth.hpp"

static InterfaceTable* ft;

namespace SCOddVoices {

OddVoicesSynth::OddVoicesSynth() {
    mCalcFunc = make_calc_function<OddVoicesSynth, &OddVoicesSynth::next>();
    next(1);
}

void OddVoicesSynth::next(int nSamples) {
    const float* input = in(0);
    const float* gain = in(1);
    float* outbuf = out(0);

    // simple gain function
    for (int i = 0; i < nSamples; ++i) {
        outbuf[i] = input[i] * gain[i];
    }
}

} // namespace SCOddVoices

PluginLoad(OddVoicesSynthUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<SCOddVoices::OddVoicesSynth>(ft, "OddVoicesSynth", false);
}
