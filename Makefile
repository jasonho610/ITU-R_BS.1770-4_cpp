# Pure targets.

.PHONY: clean clean_wav test_LKFS test_Kfilter test_Peak_Normalize test_Loudness_Normalize

BS1770 : LKFS.h Normalize.h vector_operation.h Wav.h main.cpp
	g++ -std=c++11 -O2 -o BS1770 main.cpp

clean_all:
	rm BS1770
	rm test_wav/Mono/*kfiltered.wav test_wav/Stereo/*kfiltered.wav test_wav/Mono/*normalized.wav test_wav/Stereo/*normalized.wav
		
clean_kfilt_wav:
	rm test_wav/Mono/*kfiltered.wav test_wav/Stereo/*kfiltered.wav

clean_norm_wav:
	rm test_wav/Mono/*normalized.wav test_wav/Stereo/*normalized.wav

test_LKFS:
	./BS1770 -l test_wav/Mono/speech-48k.wav
	./BS1770 -l test_wav/Stereo/Fkj\ -\ Ylang\ Ylang.wav
	
test_Kfilter:
	./BS1770 -k test_wav/Mono/speech-48k.wav 
	./BS1770 -k test_wav/Stereo/Fkj\ -\ Ylang\ Ylang.wav

test_Peak_Normalize:
	./BS1770 -n -p -3.0 test_wav/Mono/speech-48k.wav 
	./BS1770 -n -p -3.0 test_wav/Stereo/Fkj\ -\ Ylang\ Ylang.wav

test_Loudness_Normalize:
	./BS1770 -n -l -30.0 test_wav/Mono/speech-48k.wav 
	./BS1770 -n -l -24.0 test_wav/Stereo/Fkj\ -\ Ylang\ Ylang.wav
