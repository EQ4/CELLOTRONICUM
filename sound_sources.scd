SynthDef.new(\eff_playbuf, {
	arg outbus=0, bufnum=0;
	Out.ar( outbus, PlayBuf.ar(1, bufnum, BufRateScale.kr(bufnum)));
}).add;