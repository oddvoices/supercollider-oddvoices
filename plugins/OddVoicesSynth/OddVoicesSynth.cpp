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

    ClearUnitOutputs(this, 1);
}

void OddVoicesLoad::next(int nSamples) {
    ClearUnitOutputs(this, nSamples);
}

OddVoicesSynth::OddVoicesSynth() {
    clear(1);

    int voiceIndex = in0(0);
    if (!((0 <= voiceIndex) && (voiceIndex < g_voices.size()))) {
        mCalcFunc = make_calc_function<OddVoicesSynth, &OddVoicesSynth::clear>();
        m_synth = nullptr;
        return;
    }
    auto database = g_voices[voiceIndex];
    m_synth = new oddvoices::Synth(48000, database);

    mCalcFunc = make_calc_function<OddVoicesSynth, &OddVoicesSynth::next>();
}

OddVoicesSynth::~OddVoicesSynth() {
    if (m_synth) {
        delete m_synth;
    }
}

void OddVoicesSynth::clear(int nSamples) {
    ClearUnitOutputs(this, nSamples);
}

void OddVoicesSynth::next(int nSamples) {
    bool gate = in0(1) > 0;
    float freq = in0(2);
    bool segmentTrig = in0(3) > 0;
    int segmentIndex = in0(4);
    float* outbuf = out(0);

    if (gate && !m_lastGate) {
        m_synth->noteOn();
    }
    if (!gate && m_lastGate) {
        m_synth->noteOff();
    }
    if (segmentTrig && !m_lastSegmentTrig) {
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
    ft = inTable;
    registerUnit<SCOddVoices::OddVoicesSynth>(ft, "OddVoicesSynth", false);
    registerUnit<SCOddVoices::OddVoicesLoad>(ft, "OddVoicesLoad", false);
}
