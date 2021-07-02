//
//  Normalize.h
//  LKFS
//
//  Created by 何冠勳 on 2021/5/24.
//

#ifndef Normalize_h
#define Normalize_h
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "Wav.h"
#include "vector_operation.h"

using namespace std;

// PEAK NORMAL
/*===================peak_normalize func. Stereo overload====================*/
void peak_normalize(Stereo_Wav &wavein, Stereo_Wav &waveout, double target) {
    vector<double> left(wavein.left_data.begin(), wavein.left_data.end());
    vector<double> right(wavein.right_data.begin(), wavein.right_data.end());
    
    double current_peak = max(abs_max_element(left), abs_max_element(right));
    cout << "current_peak : " << current_peak << endl;
    double gain = pow(10.0, target/20.0) / current_peak;
    scalar(left, gain);
    scalar(right, gain);
    
    double normalized_peak = max(abs_max_element(left), abs_max_element(right));
    cout << "normalized_peak : " << normalized_peak << endl;
    
    waveout.left_data.clear(); waveout.left_data = left;
    waveout.right_data.clear(); waveout.right_data = right;
}
/*===================peak_normalize func. Mono overload====================*/
void peak_normalize(Mono_Wav &wavein, Mono_Wav &waveout, double target) {
    vector<double> data(wavein.data.begin(), wavein.data.end());
    
    double current_peak = abs_max_element(data);
    cout << "current_peak : " << current_peak << endl;
    double gain = pow(10.0, target/20.0) / current_peak;
    scalar(data, gain);
    
    //double normalized_peak = *max_element(data.begin(), data.end());
    double normalized_peak = abs_max_element(data);
    cout << "normalized_peak : " << normalized_peak << endl;
     
    waveout.data.clear(); waveout.data = data;
}

// LOUD NORMAL
/*===================loudness_normalize func. Stereo overload====================*/
void loudness_normalize(Stereo_Wav &wavein, Stereo_Wav &waveout, double input_loudness, double target_loudness) {
    vector<double> left(wavein.left_data.begin(), wavein.left_data.end());
    vector<double> right(wavein.right_data.begin(), wavein.right_data.end());
    
    double delta = target_loudness - input_loudness;
    double gain = pow(10.0, delta/20.0);
    scalar(left, gain);
    scalar(right, gain);
    
    
    double normalized_peak = max(abs_max_element(left), abs_max_element(right));
    cout << "normalized_peak : " << normalized_peak << endl;
    
    waveout.left_data.clear(); waveout.left_data = left;
    waveout.right_data.clear(); waveout.right_data = right;
}
/*===================loudness_normalize func. Mono overload====================*/
void loudness_normalize(Mono_Wav &wavein, Mono_Wav &waveout, double input_loudness, double target_loudness) {
    vector<double> data(wavein.data.begin(), wavein.data.end());
   
    double delta = target_loudness - input_loudness;
    double gain = pow(10.0, delta/20.0);
    scalar(data, gain);
    
    double normalized_peak = abs_max_element(data);
    cout << "normalized_peak : " << normalized_peak << endl;
    
    waveout.data.clear(); waveout.data = data;
}

#endif /* Normalize_h */
