// PluginOddVoicesSynth.cpp
// Nathan Ho (nathan.waikin.ho@gmail.com)

#include "SC_PlugIn.hpp"
#include "OddVoicesSynth.hpp"

static InterfaceTable* ft;

namespace SCOddVoices {

std::vector<std::shared_ptr<oddvoices::Database>> g_voices;

OddVoicesSynth::OddVoicesSynth() {
    clear(1);

    int voiceIndex = in0(0);
    if (!((0 <= voiceIndex) && (voiceIndex < g_voices.size()))) {
        mCalcFunc = make_calc_function<OddVoicesSynth, &OddVoicesSynth::clear>();
        m_synth = nullptr;
        return;
    }
    m_database = g_voices[voiceIndex];
    m_synth = new oddvoices::Synth(48000, m_database);

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
    bool queueSegment = in0(3) > 0;
    int segmentIndex = in0(4);
    float phonemeSpeed = in0(5);
    float formantShift = in0(6);
    float* outbuf = out(0);

    if (gate && !m_lastGate) {
        m_synth->noteOn();
    }
    if (!gate && m_lastGate) {
        m_synth->noteOff();
    }
    if (queueSegment) {
        m_synth->queueSegment(segmentIndex % m_database->getNumSegments());
    }

    m_synth->setFrequency(freq);
    m_synth->setPhonemeSpeed(phonemeSpeed);
    m_synth->setFormantShift(formantShift);

    for (int i = 0; i < nSamples; ++i) {
        outbuf[i] = m_synth->process() / 32767.0f;
    }

    m_lastGate = gate;
}

void load(
    World *inWorld
    , void* inUserData
    , sc_msg_iter *args
    , void *replyAddr
)
{
    auto voice = std::make_shared<oddvoices::Database>(args->gets());
    g_voices.push_back(voice);
}

} // namespace SCOddVoices

PluginLoad(OddVoicesUGens) {
    ft = inTable;
    registerUnit<SCOddVoices::OddVoicesSynth>(ft, "OddVoicesSynth", false);

    DefinePlugInCmd("oddvoices_load", SCOddVoices::load, nullptr);
}
