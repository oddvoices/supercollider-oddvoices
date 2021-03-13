// PluginOddVoicesSynth.hpp
// Nathan Ho (nathan.waikin.ho@gmail.com)

#pragma once

#include "SC_PlugIn.hpp"
#include "liboddvoices.hpp"

namespace SCOddVoices {

class OddVoicesLoad : public SCUnit {
public:
    OddVoicesLoad();

private:
    void next(int nSamples);
};

class OddVoicesSynth : public SCUnit {
public:
    OddVoicesSynth();
    ~OddVoicesSynth();

private:
    void next(int nSamples);
    void clear(int nSamples);

    bool m_lastGate;
    bool m_lastSegmentTrig;

    oddvoices::Synth* m_synth;
};

} // namespace SCOddVoices
