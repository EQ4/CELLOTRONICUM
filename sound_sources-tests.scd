//SHEPHARD CYCLE TONE
// RECORDING
b = Buffer.read(s, ~p +/+ "zBrody1.wav");
b.bufnum.postln;

s.prepareForRecord(~p +/+ "shephard_tone-4.aiff");
s.record;

x = Synth.new(\eff_shepard_cycle_tone, [ \outbus, 0, \inbus, 17, \amp, 0.4, \freq_bus, 16]);
y = Synth.new(\eff_pitch, [\inbus, 17, \outbus, 16]);
z = Synth.new(\eff_playbuf, [\outbus, 17, \bufnum, 1]);
s.stopRecording;

//TEST 1
SynthDef.new(\test_sin,{
	arg outbus=0, out_freq;
	var x = XLine.kr(10, 1760*8,30);
	Out.ar(outbus, SinOsc.ar(x));
	Out.kr(out_freq,x);
}).add;
x = Synth.new(\eff_shepard_cycle_tone, [ \outbus, 0, \inbus, 17, \amp, 0.4, \freq_bus, 16]);
z = Synth.new(\test_sin, [\outbus, 17, \out_freq, 16]);

//TEST 2
x = Synth.new(\eff_shepard_cycle_tone, [ \outbus, 0, \inbus, 8, \amp, 0.4, \freq_bus, 16]);
y = Synth.new(\eff_pitch, [\inbus, 8, \outbus, 16]);

//Notatka: Pitch detector ma jakies problemy z sinusami o okreslonej freq.

//TEST 3
x = Synth.new(\eff_shepard_tone, [ \outbus, 0, \amp, 0.4, \freq_bus, 8]);
x.set(\freq,150);

//PIŁA PO PRZEJŚCIACH
// RECORDING
b = Buffer.read(s, ~p +/+ "zBrody1.wav");
b.bufnum.postln;

s.prepareForRecord(~p +/+ "pitch_follow-4.aiff");
s.record;

x = Synth.new(\eff_pila_po_przejsciach, [ \outbus, 0, \inbus, 17, \freq_bus, 16]);
y = Synth.new(\eff_pitch, [\inbus, 17, \outbus, 16]);
z = Synth.new(\eff_playbuf, [\outbus, 17, \bufnum, 1]);
s.stopRecording;

// AY & YM
// RECORDING
s.prepareForRecord(~p +/+ "ym-gain25+tartinipitch.aiff");
s.record;

x = Synth.new(\eff_ay, [\freq_bus, 16, \amp_bus, 18, \outbus, 0]);
y = Synth.new(\eff_pitch, [\inbus, 17, \outbus, 16]);
a = Synth.new(\eff_amplitude, [\inbus, 17, \outbus, 18]);
z = Synth.new(\eff_playbuf, [\outbus, 17, \bufnum, 0]);

x = Synth.new(\eff_ym, [\freq_bus, 16, \amp_bus, 18, \outbus, 0]);
a = Synth.new(\eff_amplitude, [\inbus, 17, \outbus, 18]);
y = Synth.new(\eff_tartini_pitch, [\inbus, 17, \outbus, 16]);
z = Synth.new(\eff_playbuf, [\outbus, 17, \bufnum, 0]);

s.stopRecording;