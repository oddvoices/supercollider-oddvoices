// PluginOddVoicesSynth.hpp
// Nathan Ho (nathan.waikin.ho@gmail.com)

#pragma once

#include "SC_PlugIn.hpp"

namespace SCOddVoices {

class OddVoicesSynth : public SCUnit {
public:
    OddVoicesSynth();

    // Destructor
    // ~OddVoicesSynth();

private:
    // Calc function
    void next(int nSamples);

    // Member variables
};

} // namespace SCOddVoices
