OddVoicesLoad : UGen {
	*kr {
		^this.multiNew('control');
	}
}

OddVoicesSynth : UGen {
	*ar { |voice = 0, gate = 0, frequency = 440, segmentTrig = 0, segmentIndex = 0|
		^this.multiNew('audio', voice, gate, frequency, segmentTrig, segmentIndex);
	}
}
