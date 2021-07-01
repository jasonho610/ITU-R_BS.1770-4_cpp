//
//  main.cpp
//  LKFS
//
//  Created by 何冠勳 on 2021/4/19.
//

#include <iostream>
#include <ctime>
#include <vector>
#include <string>
#include "vector_operation.h"
#include "Wav.h"
#include "LKFS.h"
#include "Normalize.h"
using namespace std;

void help_instruction() {
    string help_text = "\
=========================================================\n\
|| Instruction help:                                   ||\n\
||     $ \033[32m./<exe> -h\033[0m                                    ||\n\
|| <exe> sub with the file name of your .exe           ||\n\
||                                                     ||\n\
|| Compute the LKFS of a wavefile:                     ||\n\
||     $ \033[32m./<exe> -l <wav>\033[0m                              ||\n\
|| <exe> sub with the file name of your .exe           ||\n\
|| <wav> sub with the file name of your .wav           ||\n\
||                                                     ||\n\
|| K-filtered your wavefile:                           ||\n\
||     $ \033[32m./<exe> -k <wav>\033[0m                              ||\n\
|| <exe> sub with the file name of your .exe           ||\n\
|| <wav> sub with the file name of your .wav           ||\n\
|| output wav is named <wav>_kfiltered.wav             ||\n\
||                                                     ||\n\
|| Peak normalize your audio:                          ||\n\
||     $ \033[32m./<exe> -n -p <target> <wav>\033[0m                  ||\n\
|| <exe> sub with the file name of your .exe           ||\n\
|| <target> sub with your target loudness, should      ||\n\
||          be negative double (in LKFS)               ||\n\
|| <wav> sub with the file name of your .wav           ||\n\
|| output wav is named <wav>_pk_normalized.wav         ||\n\
||                                                     ||\n\
|| Loudness normalize your audio:                      ||\n\
||     $ \033[32m./<exe> -n -l <target> <wav>\033[0m                  ||\n\
|| <exe> sub with the file name of your .exe           ||\n\
|| <target> sub with your target loudness, should      ||\n\
||          be negative double (in LKFS)               ||\n\
|| <wav> sub with the file name of your .wav           ||\n\
|| output wav is named <wav>_ld_normalized.wav         ||\n\
||                                                     ||\n\
=========================================================\n";
    cout << help_text;
}

bool compute_LKFS(string infname);
bool filter_audio(string infname);
bool normalize(string method, string infname, double target);

int main(int argc, const char* argv[]) {
    /*------DEBUG SECTION------*/
    /*
    vector<string> infname;
    infname.push_back("test_wav/Stereo/Fkj - Ylang Ylang.wav");
    infname.push_back("test_wav/Mono/speech(48k).wav");
    
    for(int i=0;i<infname.size();i++) {
        //compute_LKFS(infname[i]);              // TESTED, "speech(48k)" but has diff 0.018691892 with py
        //filter_audio(infname[i]);              // TESTED
        normalize("-p", infname[i], -24.0);    // TESTED
        normalize("-l", infname[i], -24.0);    // TESTED, "speech(48k)" but has diff 0.361737893 with py
    }
    return 0;*/
    /*------DEBUG SECTION END------*/
    
    if (argc==1) {
        cout << "Acquire some arguments." << endl;
        help_instruction();
        return 1;
    }
    
    vector<string> Argvs(argv, argv+argc);
    bool err = false;
    
    if (Argvs[1]=="-h") {
        help_instruction();
    }
    else if (Argvs[1]=="-l") {
        string infname = Argvs[2];
        err = compute_LKFS(infname);
    }
    else if (Argvs[1]=="-k") {
        string infname = Argvs[2];
        err = filter_audio(infname);
    }
    else if ((Argvs[1]=="-n")&&((Argvs[2]=="-p")||(Argvs[2]=="-l"))) {
        string method = Argvs[2];
        double target = stod(Argvs[3]);
        string infname = Argvs[4];
        err = normalize(method, infname, target);
    }
    else {
        cout << "No such command, ";
        cout << "try $\033[32m./<exe> -h\033[0m for help." << endl;
    }
    if (err) cerr << "Use the imformations above to debug." << endl;
    
    return 0;
}

bool compute_LKFS(string infname) {
    unsigned short NumCh = Check_Stereo_Mono(infname);
    if (NumCh==false) {
        cerr << "Please check your file position or file name." << endl;
        return true;
    }
    
    if (NumCh==1) {
        Mono_Wav wavein;
        double loudness = 0.0;
        
        if (!wavein.readfile(infname)) {
            cerr << "Please check your file position or file name." << endl;
            return true;
        }
        
        double fs = wavein.header.get_SampleRate();
        loudness = integrated_loudness(wavein, fs);
        cout << "Integrated loudness : " << setprecision(17) << loudness << endl;
    }
    else if (NumCh==2) {
        Stereo_Wav wavein;
        double loudness = 0.0;
        
        if (!wavein.readfile(infname)) {
            cout << "Please check your file position or file name." << endl;
            return true;
        }
        
        double fs = wavein.header.get_SampleRate();
        loudness = integrated_loudness(wavein, fs);
        cout << "Integrated loudness : " << setprecision(17) << loudness << endl;
    }
    else {
        cout << "Only support \033[31mMono/Stereo, signed-16-bit, PCM\033[0m wav now." << endl;
    }
    
    return false;
}

bool normalize(string method, string infname, double target = -24.0) {
    unsigned short NumCh = Check_Stereo_Mono(infname);
    if (NumCh==false) {
        cerr << "Please check your file position or file name." << endl;
        return true;
    }
    
    if (NumCh==1) {
        Mono_Wav wavein;
        string outfname;
        
        if (!wavein.readfile(infname)) {
            cout << "Please check your file path or file name." << endl;
            return true;
        }
        
        Mono_Wav waveout(wavein.header.get_SampleRate(), wavein.header.get_BitsPerSample(), wavein.data.size());
        infname.erase(infname.end()-4, infname.end());
        if (method=="-p") {
            peak_normalize(wavein, waveout, target);
            outfname = infname + "_pk_normalized.wav";
        }
        else if (method=="-l") {
            double fs = wavein.header.get_SampleRate();
            double loudness = integrated_loudness(wavein, fs);
            loudness_normalize(wavein, waveout, loudness, target);
            outfname = infname + "_ld_normalized.wav";
        }
        
        waveout.writefile(outfname);
    }
    else if (NumCh==2) {
        Stereo_Wav wavein;
        string outfname;
        
        if (!wavein.readfile(infname)) {
            cout << "Please check your file path or file name." << endl;
            return true;
        }
        
        Stereo_Wav waveout(wavein.header.get_SampleRate(), wavein.header.get_BitsPerSample(), wavein.left_data.size());
        
        infname.erase(infname.end()-4, infname.end());
        if (method=="-p") {
            peak_normalize(wavein, waveout, target);
            outfname = infname + "_pk_normalized.wav";
        }
        else if (method=="-l") {
            double fs = wavein.header.get_SampleRate();
            double loudness = integrated_loudness(wavein, fs);
            loudness_normalize(wavein, waveout, loudness, target);
            outfname = infname + "_ld_normalized.wav";
        }
        waveout.writefile(outfname);
    }
    else {
        cout << "Only support \033[31mMono/Stereo, signed-16-bit, PCM\033[0m wav now." << endl;
    }
    
    return false;
}

bool filter_audio(string infname) {
    unsigned short NumCh = Check_Stereo_Mono(infname);
    if (NumCh==false) {
        cerr << "Please check your file position or file name." << endl;
        return true;
    }
    
    if (NumCh==1) {
        Mono_Wav wavein;
        if(!wavein.readfile(infname)) {
            cout << "Please check your file position or file name." << endl;
            return true;
        }
        
        double fs = wavein.header.get_SampleRate();
        vector<double> data_in(wavein.data.begin(), wavein.data.end());
        vector<double> data_out(data_in.size(), 0.0);
        k_filter(data_in, fs, data_out);
        
        infname.erase(infname.end()-4, infname.end());
        string outfname = infname + "_kfiltered.wav";
        Mono_Wav waveout(wavein.header.get_SampleRate(), wavein.header.get_BitsPerSample(), data_out.size(), data_out);
        waveout.writefile(outfname);
        
    }
    else if (NumCh==2) {
        Stereo_Wav wavein;
        if (!wavein.readfile(infname)) {
            cout << "Please check your file position or file name." << endl;
            return true;
        }
        double fs = wavein.header.get_SampleRate();
        vector<double> left_in(wavein.left_data.begin(), wavein.left_data.end());
        vector<double> right_in(wavein.right_data.begin(), wavein.right_data.end());
        vector<double> left_out(left_in.size(), 0.0);
        vector<double> right_out(right_in.size(), 0.0);
        k_filter(left_in, fs, left_out);
        k_filter(right_in, fs, right_out);
        
        infname.erase(infname.end()-4, infname.end());
        string outfname = infname + "_kfiltered.wav";
        Stereo_Wav waveout(wavein.header.get_SampleRate(), wavein.header.get_BitsPerSample(), left_out.size(), left_out, right_out);
        waveout.writefile(outfname);
    }
    else {
        cout << "Only support \033[31mMono/Stereo, signed-16-bit, PCM\033[0m wav now." << endl;
    }
    
    return false;
}
