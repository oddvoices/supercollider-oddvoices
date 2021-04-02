OddVoicesSynth : UGen {
	*ar { |voice = 0, gate = 0, frequency = 440, queueSegment = 0, segmentIndex = 0, phonemeSpeed = 1, formantShift = 1|
		^this.multiNew('audio', voice, gate, frequency, queueSegment, segmentIndex, phonemeSpeed, formantShift);
	}
}

OddVoicesModel {
	var g2pExecutable;
	var <voiceFile;
	var <rate, <grainLength, <phonemes, <segments;
	var <segmentNames;

	*new { |voiceFile, g2pExecutable|
		^super.new.init(voiceFile, g2pExecutable);
	}

	init { |argVoiceFile, argG2pExecutable|
		voiceFile = argVoiceFile;
		g2pExecutable = argG2pExecutable;
		File.use(argVoiceFile, "rb", { |file|
			this.prRead(file);
		});
	}

	send { |server = (Server.default)|
		server.sendMsg(\cmd, "oddvoices_load", voiceFile);
	}

	queueSegments {
		|
		segmentIndices,
		node,
		server = (Server.default),
		queueSegmentControlName = \queueSegment,
		segmentIndexControlName = \segmentIndex,
		waitTime
		|

		segmentIndices.do { |segmentIndex|
			server.makeBundle(server.latency, {
				node.set(\queueSegment, 1, \segmentIndex, segmentIndex);
			});
			(waitTime ?? { 4 * (server.options.blockSize / server.sampleRate) }).wait;
		};
	}

	phonemesToSegmentIndices { |phonemes|
		var result;
		result = [];
		(phonemes.size - 1).do { |i|
			var segmentName;
			var nextPhoneme, nextPhonemeIndex;
			if (segmentNames.includes(phonemes[i])) {
				result = result.add(phonemes[i]);
			};
			nextPhonemeIndex = i + 1;
			nextPhoneme = phonemes[nextPhonemeIndex];
			{ (nextPhoneme == '-') and: { nextPhonemeIndex < phonemes.size } }.while {
				nextPhonemeIndex = nextPhonemeIndex + 1;
				nextPhoneme = phonemes[nextPhonemeIndex];
			};
			segmentName = (phonemes[i] ++ nextPhoneme).asSymbol;
			if (segmentNames.includes(segmentName)) {
				result = result.add(segmentName);
			};
		};
		^result.collect { |segmentName| this.segmentNameToSegmentIndex(segmentName) };
	}

	segmentNameToSegmentIndex { |segmentName|
		^segmentNames.indexOf(segmentName) ?? { -1 };
	}

	pronounce { |text|
		var phonemes;
		phonemes = [g2pExecutable, text].unixCmdGetStdOut;
		// Remove newline
		phonemes = phonemes[..phonemes.size - 2];
		^phonemes.split($ ).collect { |phoneme|
			phoneme.asSymbol;
		};
	}

	prRead { |file|
		var magicWord;
		var readInt32LE, readSymbol;
		var phoneme;
		var segment;

		magicWord = "ODDVOICES".ascii ++ (0 ! 3);
		if (magicWord.size.collect({ file.getChar.ascii }) != magicWord) {
			"Invalid magic word".throw;
		};

		readInt32LE = {
			file.getInt32LE;
		};

		readSymbol = {
			var result, char;
			result = [];
			char = file.getChar;
			{ char.ascii != 0 }.while {
				result = result.add(char);
				char = file.getChar;
			};
			result.join("").asSymbol;
		};

		rate = readInt32LE.();
		grainLength = readInt32LE.();

		phonemes = [];
		phoneme = readSymbol.();
		{ phoneme.asString.size != 0 }.while {
			phonemes = phonemes.add(phoneme);
			phoneme = readSymbol.();
		};

		segments = [];
		segment = (name: readSymbol.());
		{ segment.name.asString.size != 0 }.while {
			segment.numFrames = readInt32LE.();
			segment.long = readInt32LE.();
			segments = segments.add(segment);
			segment = (name: readSymbol.());
		};

		segmentNames = segments.collect { |segment| segment.name };
	}
}
