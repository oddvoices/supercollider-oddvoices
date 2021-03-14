// PluginOddVoicesSynth.hpp
// Nathan Ho (nathan.waikin.ho@gmail.com)

#pragma once

#include "SC_PlugIn.hpp"
#include "liboddvoices.hpp"

namespace SCOddVoices {

class OddVoicesSynth : public SCUnit {
public:
    OddVoicesSynth();
    ~OddVoicesSynth();

    void queueSegment(std::string segmentName);

private:
    void next(int nSamples);
    void clear(int nSamples);

    bool m_lastGate = false;
    bool m_lastSegmentTrig = false;

    oddvoices::Synth* m_synth;
    std::shared_ptr<oddvoices::Database> m_database;
};

} // namespace SCOddVoices
