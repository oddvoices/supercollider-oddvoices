OddVoicesSynth : UGen {
	*ar { |voice = 0, gate = 0, frequency = 440|
		^this.multiNew('audio', voice, gate, frequency);
	}
}
