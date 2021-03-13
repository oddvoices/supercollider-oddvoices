// PluginOddVoicesSynth.cpp
// Nathan Ho (nathan.waikin.ho@gmail.com)

#include "SC_PlugIn.hpp"
#include "OddVoicesSynth.hpp"

static InterfaceTable* ft;

namespace SCOddVoices {

std::vector<std::shared_ptr<oddvoices::Database>> g_voices;

OddVoicesLoad::OddVoicesLoad() {
    mCalcFunc = make_calc_function<OddVoicesLoad, &OddVoicesLoad::next>();

    auto voice = std::make_shared<oddvoices::Database>(
        "/home/nathan/git/oddvoices/tests/compiled-voices/nt.voice"
    );
    g_voices.push_back(voice);

    std::cout << voice->segmentToSegmentIndex("A") << std::endl;

    ClearUnitOutputs(this, 1);
}

void OddVoicesLoad::next(int nSamples) {
    ClearUnitOutputs(this, nSamples);
}

OddVoicesSynth::OddVoicesSynth() {
    mCalcFunc = make_calc_function<OddVoicesSynth, &OddVoicesSynth::next>();

    auto database = g_voices[0];
    m_synth = new oddvoices::Synth(48000, database);

    next(1);
}

void OddVoicesSynth::next(int nSamples) {
    bool gate = in0(0) > 0;
    float freq = in0(1);
    bool segmentTrig = in0(2) > 0;
    int segmentIndex = in0(3);
    float* outbuf = out(0);

    if (gate && !m_lastGate) {
        m_synth->noteOn();
    }
    if (!gate && m_lastGate) {
        m_synth->noteOff();
    }
    if (segmentTrig && m_lastSegmentTrig) {
        m_synth->queueSegment(segmentIndex % g_voices[0]->getNumSegments());
    }

    m_synth->setFrequency(freq);

    for (int i = 0; i < nSamples; ++i) {
        outbuf[i] = m_synth->process() / 32767.0f;
    }

    m_lastGate = gate;
    m_lastSegmentTrig = segmentTrig;
}

} // namespace SCOddVoices

PluginLoad(OddVoicesSynthUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<SCOddVoices::OddVoicesSynth>(ft, "OddVoicesSynth", false);
    registerUnit<SCOddVoices::OddVoicesLoad>(ft, "OddVoicesLoad", false);
}
