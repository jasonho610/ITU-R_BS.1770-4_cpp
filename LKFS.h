//
//  LKFS.h
//  LKFS
//
//  Created by 何冠勳 on 2021/5/19.
//  
// Edited by Duck on 2025/8/18 - fixed a problem in AbsGating 
//

#ifndef LKFS_h
#define LKFS_h
#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>
#include "vector_operation.h"
#include "Wav.h"
#define pi 3.14159265358979323846
using namespace std;

void filter(const vector<double> &b, const vector<double> &a, const vector<double> &x, vector<double> &y) {
    
    /*==================================================================*/
    /*=== a[0]y[n]=\sig_{i=0}^{M}b_ix[n-i] - \sig_{j=1}^{N}a_jy[n-j] ===*/
    /*==================================================================*/
    
    if (y.empty()) {
        cout << "Initialize the output vector, please." << endl;
        for(size_t i=0;i<x.size();i++) y.push_back(0.0);
    }
    
    for(int i=0;i<x.size();i++) {
        double temp = 0.0;
        /*----------------compute sigma x[n]----------------*/
        for(int j=0;j<b.size();j++) {
            if(i - j < 0) continue;
            temp += b[j] * x[i-j];
        }
        
        /*----------------compute sigma y[n]----------------*/
        for(int j=1;j<a.size();j++) {
            if(i - j < 0) continue;
            temp -= a[j]*y[i-j];
        }
        temp /= a[0];
        y[i] = temp;
    }
}

/*==========Every vector has to initial except here uses push_back()===========*/
double throw_and_mean(double threshold, vector<double> &l, vector<double> &z) {
    vector<unsigned> indicies_gated;
    for(unsigned j=0;j<l.size();j++) {
        if(l[j]>threshold) indicies_gated.push_back(j);
    }
    vector<double> z_kept;
    for(int j=0;j<indicies_gated.size();j++) {
        z_kept.push_back(z[indicies_gated[j]]);
    }
    double z_avg = vector_mean(z_kept);
    return z_avg;
}

void k_filter(const vector<double> &x, const double fs, vector<double> &y) {
    vector<double> v(x.size(), 0.0);
    /*========================pre_filter 1========================*/
    double f0 = 1681.9744509555319;
    double G  = 3.99984385397;
    double Q  = 0.7071752369554193;
    /*-----------------precompute----------------*/
    double K  = tan(pi * f0 / fs);
    double Vh = pow(10, G/20);
    double Vb = pow(Vh, 0.499666774155);
    double a0_ = 1.0 + K / Q + K * K;
    double b0 = (Vh + Vb * K / Q + K * K) / a0_;
    double b1 = 2.0 * (K * K -  Vh) / a0_;
    double b2 = (Vh - Vb * K / Q + K * K) / a0_;
    double a0 = 1.0;
    double a1 = 2.0 * (K * K - 1.0) / a0_;
    double a2 = (1.0 - K / Q + K * K) / a0_;
    /*-----------------filter-----------------*/
    vector<double> b{b0, b1,b2}, a{a0, a1, a2};
    filter(b, a, x, v);
    
    /*========================pre_filter 2========================*/
    f0 = 38.13547087613982;
    Q  = 0.5003270373253953;
    /*-----------------precompute----------------*/
    K  = tan(pi * f0 / fs);
    a0 = 1.0;
    a1 = 2.0 * (K * K - 1.0) / (1.0 + K / Q + K * K);
    a2 = (1.0 - K / Q + K * K) / (1.0 + K / Q + K * K);
    b0 = 1.0;
    b1 = -2.0;
    b2 = 1.0;
    /*-----------------filter-----------------*/
    b.clear(); b = {b0, b1,b2};
    a.clear(); a = {a0, a1, a2};
    filter(b, a, v, y);
}


void calculate_z(vector<double> &signal, double fs, size_t len, const vector<unsigned> &j_range, vector<double> &z) {
    /*========================k_filter========================*/
    vector<double> signal_filtered(len, 0.0);
    k_filter(signal, fs, signal_filtered);
    vector<double> signal_powered(signal_filtered.begin(), signal_filtered.end());
    vector_ele_pow(signal_powered, 2.0);
    // WRITE signal_filtered wav
    
    /*========================mean square========================*/
    double T_g = 0.4;
    double overlap = 0.75, step = 1 - overlap;
    
    //size_t lbound, ubound;
    for(size_t j=0;j<j_range.size();j++) {
        size_t lbound = round(fs*T_g*j_range[j]*step);
        size_t ubound = round(fs*T_g*(j_range[j]*step+1));
        if (ubound>signal_powered.size()) ubound = signal_powered.size();
        z[j] = (1.0/(T_g*fs))*vector_sum(signal_powered, lbound, ubound);
    }
}

/*===================intergrated_loudness func. Stereo overload====================*/
double integrated_loudness(Stereo_Wav &wav, double fs) {
    /*========================data normalize & copy========================*/
    size_t len = wav.left_data.size();
    vector<double> G = {1.0, 1.0};
    vector<double> left(wav.left_data.begin(), wav.left_data.end());
    vector<double> right(wav.right_data.begin(), wav.right_data.end());
    
    /*========================calculate z========================*/
    double T_g = 0.4;
    double overlap = 0.75, step = 1 - overlap;
    double T = len / fs;
    vector<unsigned> j_range = linspace(unsigned(0), unsigned(round((T-T_g)/(T_g*step))), round((T-T_g)/(T_g*step))+1);
    
    vector<double> z_left(j_range.size(), 0.0);
    vector<double> z_right(j_range.size(), 0.0);
    calculate_z(left, fs, len, j_range, z_left);
    calculate_z(right, fs, len, j_range, z_right);
    
    // left.clear(); right.clear(); // release unused vector
    
    /*========================loudness========================*/
    vector<double> l(j_range.size(), 0.0);
    for(int j=0;j<j_range.size();j++) {
        l[j] = -0.691 + 10.0*log10(G[0]*z_left[j]+G[1]*z_right[j]);
    }
    
    /*========================threshold========================*/
    // throw out anything below absolute threshold:
    double Gamma_a = -70.0;
    double z_avg_left = 0.0, z_avg_right = 0.0;
    z_avg_left = throw_and_mean(Gamma_a, l, z_left);
    z_avg_right = throw_and_mean(Gamma_a, l, z_right);
    double Gamma_r = -0.691 + 10.0*log10(G[0]*z_avg_left+G[1]*z_avg_right) - 10.0;
    //if relative threshold is under absolute, just return the abs gated result
    if(Gamma_r<Gamma_a) return Gamma_r+10.0;
    
    // throw out anything below relative threshold:
    z_avg_left = throw_and_mean(Gamma_r, l, z_left);
    z_avg_right = throw_and_mean(Gamma_r, l, z_right);
    double L_KG = -0.691 + 10.0*log10(G[0]*z_avg_left+G[1]*z_avg_right);

    return L_KG;
}

/*===================intergrated_loudness func. Mono overload====================*/
double integrated_loudness(Mono_Wav &wav, double fs) {
    /*========================data normalize & copy========================*/
    size_t len = wav.data.size();
    double G = 1.0;
    vector<double> data(wav.data.begin(), wav.data.end());
    
    /*========================calculate z========================*/
    double T_g = 0.4;
    double overlap = 0.75, step = 1 - overlap;
    double T = len / fs;
    vector<unsigned> j_range = linspace(unsigned(0), unsigned(round((T-T_g)/(T_g*step))), round((T-T_g)/(T_g*step))+1);
    
    vector<double> z(j_range.size(), 0.0);
    calculate_z(data, fs, len, j_range, z);
    
    /*========================loudness========================*/
    vector<double> l(j_range.size(), 0.0);
    for(int j=0;j<j_range.size();j++) {
        l[j] = -0.691 + 10.0*log10(G*z[j]);
    }
    
    /*========================threshold========================*/
    // throw out anything below absolute threshold:
    double Gamma_a = -70.0;
    double z_avg = 0.0;
    z_avg = throw_and_mean(Gamma_a, l, z);
    double Gamma_r = -0.691 + 10.0*log10(G*z_avg) - 10.0;
    //if relative threshold is under absolute, just return the abs gated result
    if(Gamma_r<Gamma_a) return Gamma_r+10.0;

    // throw out anything below relative threshold:
    z_avg = throw_and_mean(Gamma_r, l, z);
    double L_KG = -0.691 + 10.0*log10(G*z_avg);

    return L_KG;
}

#endif /* LKFS_h */
