OddVoicesSynth : UGen {
	*ar { |voice = 0, gate = 0, frequency = 440, queueSegment = 0, segmentIndex = 0|
		^this.multiNew('audio', voice, gate, frequency, queueSegment, segmentIndex);
	}
}
