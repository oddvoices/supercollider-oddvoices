OddVoicesLoad : UGen {
	*kr {
		^this.multiNew('control');
	}
}

OddVoicesSynth : UGen {
	*ar { |gate = 0, frequency = 440, segmentTrig = 0, segmentIndex = 0|
		^this.multiNew('audio', gate, frequency, segmentTrig, segmentIndex);
	}
}
