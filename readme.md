# ITU-R_BS.1770-4_cpp

## Introduction
This project implements the standard ITU-R BS.1770-4 in C/C++, along with other applications relevantly. The ITU-R BS.1770-4 specifies audio measurement algorithms for the purpose of determining subjective programme loudness, and true-peak signal level. 

## Usage
First download 7 files :
1. test_wav file
2. vector_operation.h
3. Wav.h
4. LKFS.h
5. Normalize.h
6. main.cpp
7. Makefile

Store them under the same directory, then open your terminal
```
$ make
```
This automatically makes main.cpp compiled. There are four main applications.
Compute integrated loudness :
```
$ ./BS1770 -l <wav>
```
K-filter the audio :
```
$ ./BS1770 -k <wav>
```
Peak normalize the audio :
```
$ ./BS1770 -n -p <target> <wav>
```
Loudness normalize the audio :
```
$ ./BS1770 -n l <target> <wav>
```
:::info
<target> sub with your target loudness, should be negative double (in LKFS)
<wav>    sub with the file name of your .wav
:::

To delete produced file :
```
$ make clean_all
```
To delete k-filtered wav :
```
$ make clean_kfilt_wav
```
To delete normalized k-filtered wav :
```
$ make clean_norm_wav
```
Test functions by sample wav:
```
$ make test_LKFS
```
```
$ make test_Kfilter
```
```
$ make test_Peak_Normalize
```
```
$ make test_Loudness_Normalize
```

further help :
```
$ ./BS1770 -h
```
## References
The code mainly takes references from [pyloudnorm](https://github.com/csteinmetz1/pyloudnorm)<font size=1>[1]</font> and [loudness.py](https://github.com/BrechtDeMan/loudness.py)<font size=1>[2]</font>.

<font size=1>[1]</font>Steinmetz, Christian J. and Reiss, Joshua D., "pyloudnorm : a simple yet flexible loudness meter in Python"

<font size=1>[2]</font>Brecht De Man, "Evaluation of Implementations of the EBU R128 Loudness Measurement," 145th International Convention of the Audio Engineering Society, October 2018.

## Authors

JasonHo, undergraduate, major in Computer Science and Music.

CyChiang, professor, superviser.

## Original Hackmd
https://hackmd.io/@jasonho610/SyOOtVH9O