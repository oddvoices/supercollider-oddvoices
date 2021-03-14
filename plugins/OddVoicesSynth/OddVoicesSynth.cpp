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
    float* outbuf = out(0);

    if (gate && !m_lastGate) {
        m_synth->noteOn();
    }
    if (!gate && m_lastGate) {
        m_synth->noteOff();
    }

    m_synth->setFrequency(freq);

    for (int i = 0; i < nSamples; ++i) {
        outbuf[i] = m_synth->process() / 32767.0f;
    }

    m_lastGate = gate;
}

void OddVoicesSynth::queueSegment(std::string segmentName) {
    m_synth->queueSegment(
        m_database->segmentToSegmentIndex(segmentName)
    );
}

void load(
    World *inWorld
    , void* inUserData
    , sc_msg_iter *args
    , void *replyAddr
)
{
    auto voice = std::make_shared<oddvoices::Database>(
        "/home/nathan/git/oddvoices/tests/compiled-voices/nt.voice"
    );
    g_voices.push_back(voice);
}

void queueSegment(OddVoicesSynth* unit, sc_msg_iter *args)
{
    std::string segmentName = args->gets();
    unit->queueSegment(segmentName);
}

} // namespace SCOddVoices

PluginLoad(OddVoicesUGens) {
    ft = inTable;
    registerUnit<SCOddVoices::OddVoicesSynth>(ft, "OddVoicesSynth", false);

    DefinePlugInCmd("oddvoices_load", SCOddVoices::load, nullptr);
    DefineUnitCmd("OddVoicesSynth", "queueSegment", (UnitCmdFunc)SCOddVoices::queueSegment);
}
