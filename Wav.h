//
//  Wav.h
//  DSP HW5
//
//  Created by 何冠勳 on 2021/1/29.
//
#ifndef Wav_h
#define Wav_h
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "vector_operation.h"
using namespace std;

typedef struct WaveHeader {
protected:
    /*===================Format===================*/
    // riff wave header
    char ChunkID[4] = {'R','I','F','F'};
    unsigned ChunkSize;        // 0 ~ FFFF,FFFF
    char Format[4] = {'W','A','V','E'};
    // fmt subchunk
    char SubChunk1ID[4] = {'f','m','t',' '};
    unsigned SubChunk1Size;    // 0 ~ FFFF,FFFF
    unsigned short AudioFormat;    // 0 ~ FFFF
    unsigned short NumChannels;    // 0 ~ FFFF
    unsigned SampleRate;       // 0 ~ FFFF,FFFF
    unsigned ByteRate;         // 0 ~ FFFF,FFFF
    unsigned short BlockAlign;     // 0 ~ FFFF
    unsigned short BitsPerSample;  // 0 ~ FFFF
    // data subchunk
    char SubChunk2ID[4] = {'d','a','t','a'};
    unsigned SubChunk2Size;    // 0 ~ FFFF,FFFF
    
public:
    /*===================Constructor===================*/
    // Empty Constructor
    WaveHeader() {}
    // Explicit Constructor
    WaveHeader(unsigned short const NC, unsigned const SR, unsigned short const BPS, unsigned NoS) {
        AudioFormat = 1;                  // 1 for PCM...
        SampleRate = SR;
        NumChannels = NC;                 // 1 for Mono, 2 for Stereo
        BitsPerSample = BPS;
        ByteRate = (SampleRate * NumChannels * BitsPerSample)/8;
        BlockAlign = (NumChannels * BitsPerSample)/8;
        SubChunk2Size = (NoS * NumChannels * BitsPerSample)/8;
        SubChunk1Size = 16;               // 16 for PCM
        ChunkSize = 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size);
    }
    
    /*===================Destructor===================*/
    //~WaveHeader() { cout << "Waveheader killed" << endl; }
    
    /*===================Get Info===================*/
    unsigned get_ChunkSize() { return ChunkSize; }
    unsigned get_SubChunk1Size() { return SubChunk1Size; }
    unsigned short get_AudioFormat() { return AudioFormat; }
    unsigned short get_NumChannels() { return NumChannels; }
    unsigned get_SampleRate() { return SampleRate; }  // WARNING! If getting f_s is for computing,
                                                      // please use double to contain.
                                                      // e.g: double fs = wav.header.get_SampleRate;
    unsigned get_ByteRate() { return ByteRate; }
    unsigned short get_BlockAlign() { return BlockAlign; }
    unsigned short get_BitsPerSample() { return BitsPerSample; }
    unsigned get_SubChunk2Size() { return SubChunk2Size; }
    
    /*===================Clear===================*/
    void clear() {
        ChunkSize = 0; SubChunk1Size = 0; AudioFormat = 0; NumChannels = 0; SampleRate = 0;
        ByteRate = 0; BlockAlign = 0; BitsPerSample = 0; SubChunk2Size = 0;
    }
} WaveHeader;

typedef struct Stereo_Wav {
    /*===================Format===================*/
    WaveHeader header;
    vector<double> left_data;
    vector<double> right_data;

    /*===================Constructor===================*/
    // Empty Constructor
    Stereo_Wav() {}
    // Implicit Constructor by Waveheader
    Stereo_Wav(unsigned const SR, unsigned short const BPS, unsigned NoS):header(2, SR, BPS, NoS) {
        for(size_t i=0;i<NoS;i++) {
            left_data.push_back(0);
            right_data.push_back(0);
        }
    }
    // Explicit Constructor
    Stereo_Wav(unsigned const SR, unsigned short const BPS, unsigned NoS, vector<double> ld, vector<double> rd):header(2, SR, BPS, NoS), left_data(ld), right_data(rd) {
        if(left_data.size()!=right_data.size()) {
            cout << "Warning: audio left_data and audio right_data are not the same size." << endl;
        }
        if(NoS!=left_data.size()) {
            cout << "Warning: Number of samples and audio left_data size are not the same." << endl;
        }
        if(NoS!=right_data.size()) {
            cout << "Warning: Number of samples and audio right_data size are not the same." << endl;
        }
    }
    
    /*===================Destructor===================*/
    //~Stereo_Wav() { cout << "Stereo_Wav killed" << endl; }
    
    /*===================Clear===================*/
    void clear() {
        header.clear();
        left_data.clear();
        right_data.clear();
    }
    
    /*===================File Operation===================*/
    /*===================Read===================*/
    bool readfile(string filename) {
        ifstream infile;
        WaveHeader hd;
        vector<short> data_s;
        
        infile.open(filename, ofstream::binary|ios::in);
        if (!infile.is_open()) {
            cerr << "Could not open the file." << endl;
            return false;
        }
        
        infile.read((char *)&hd, sizeof(hd));
        //cout << header.SampleRate << endl;
        header = hd;

        while(!infile.eof()) {
            short temp;        // data can't be greater than FFFF(65535).
            infile.read((char*)&temp, sizeof(temp));
            data_s.push_back(temp);
        }
        infile.close();
        vector<double> data_d(data_s.begin(), data_s.end());
        scalar(data_d, 1.0/32767.0);   // normalize to [-1,1] by 0xFFFF
        
        /*------------------------------------*/
        /* Change data length here for testing*/
        for(size_t i=0;i<data_d.size();i=i+2) {
            left_data.push_back(data_d[i]);
            right_data.push_back(data_d[i+1]);
        }
        return true;
    }
    
    /*===================Write===================*/
    bool writefile(string filename) {
        ofstream outfile;
        outfile.open(filename, ofstream::binary|ofstream::out);
        if (!outfile.is_open()) {
            cerr << "Could not open the file." << endl;
            return false;
        }
        
        outfile.write((char*)&header, sizeof(header));
        
        double peak = max(abs_max_element(left_data), abs_max_element(right_data));
        if(peak>=1.0) {
            cout << "Possible clipped samples in output" << endl;
            cout << "(peak : " << peak << ")" << endl;
        }
        
        scalar(left_data, 32767.0);   // normalize back to [0~FFFF]
        scalar(right_data, 32767.0);  // normalize back to [0~FFFF]
        vector<short> left_s(left_data.begin(), left_data.end());
        vector<short> right_s(right_data.begin(), right_data.end());
        
        for(size_t i=0;i<left_s.size();i++) {
            outfile.write((char*)&left_s[i], sizeof(left_s[i]));
            outfile.write((char*)&right_s[i], sizeof(right_s[i]));
        }
        outfile.close();
        cout << filename << " is wirtten successfully." << endl << endl;
        
        return false;
    }
} Stereo_Wav;

typedef struct Mono_Wav {
    /*===================Format===================*/
    WaveHeader header;
    vector<double> data;
    
    /*===================Constructor===================*/
    // Empty Constructor
    Mono_Wav() {}
    // Implicit Constructor by Waveheader
    Mono_Wav(unsigned const SR, unsigned short const BPS, unsigned NoS):header(1, SR, BPS, NoS) {
        for(size_t i=0;i<NoS;i++) {
            data.push_back(0);
        }
    }
    // Explicit Constructor
    Mono_Wav(unsigned const SR, unsigned short const BPS, unsigned NoS, vector<double> d):header(1, SR, BPS, NoS), data(d) {
        if(NoS!=data.size()) {
            cout << "Warning: Number of samples and audio data size are not the same." << endl;
        }
    }
    
    /*===================Clear===================*/
    void clear() {
        header.clear();
        data.clear();
    }
    
    /*===================File Operation===================*/
    /*===================Read===================*/
    bool readfile(string filename) {
        ifstream infile;
        WaveHeader hd;
        vector<short> data_s;
        
        infile.open(filename, ofstream::binary|ios::in);
        if (!infile.is_open()) {
            cerr << "Could not open the file." << endl;
            return false;
        }
        
        infile.read((char *)&hd, sizeof(hd));
        //cout << header.SampleRate << endl;
        header = hd;

        while(!infile.eof()) {
            short temp;        // data can't be greater than FFFF(65535).
            infile.read((char *)&temp, sizeof(temp));
            data_s.push_back(temp);
        }
        infile.close();
        vector<double> data_d(data_s.begin(), data_s.end());
        scalar(data_d, 1.0/32767.0);   // normalize to [-1,1] by 0xFFFF
        data = data_d;
        
        return true;
    }
    /*===================Write===================*/
    bool writefile(string filename) {
        ofstream outfile;
        outfile.open(filename, ofstream::binary|ofstream::out);
        if (!outfile.is_open()) {
            cerr << "Could not open the file." << endl;
            return false;
        }
        
        outfile.write((char*)&header, sizeof(header));
        
        double peak = abs_max_element(data);
        if(peak>=1.0) {
            cout << "Possible clipped samples in output." << endl;
            cout << "(peak : " << peak << ")" << endl;
        }
        scalar(data, 32767.0);   // normalize back to [0~FFFF]
        vector<short> data_s(data.begin(), data.end());
        
        for(size_t i=0;i<data_s.size();i++) {
            outfile.write((char*)&data_s[i], sizeof(data_s[i]));
        }
        outfile.close();
        cout << filename << " is wirtten successfully." << endl << endl;
        
        return false;
    }
    
} Mono_Wav;

unsigned short Check_Stereo_Mono(string filename) {
    ifstream infile;
    WaveHeader hd;
    
    infile.open(filename, ofstream::binary|ios::in);
    if (!infile.is_open()) {
        cerr << "Could not open the file." << endl;
        return false;
    }
    
    infile.read((char*)&hd, sizeof(hd));
    infile.close();
    
    if ((hd.get_AudioFormat()!=1)||(hd.get_BitsPerSample()!=16)) {
        return 6;
    }
    
    return hd.get_NumChannels();
}

#endif /* Wav_h */
