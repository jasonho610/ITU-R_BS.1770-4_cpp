# Audio Normalization - LKFS
<center> <font size=2> Jason [2021/03] </font> </center>

## Source
- [Audio Normalization 的二三事（上）](https://medium.com/kkstream/%E6%9C%89%E9%97%9Caudio-normalization%E5%85%A9%E4%B8%89%E4%BA%8B-dca62497e197)
>聲音以dB為單位，也就是大家所熟知的分貝 $$ L_p=20\log_{10}{\frac{P_{rms}}{P_{ref}}} $$ 但是人腦所知覺的音量大小並不是單純只跟音壓/振幅有關，聲音的頻率其實也會影響，即便兩個相同振幅的聲音，若是音頻不同，聽起來的音量可能會是不一樣的。

>因此，基於這樣的現象，出現了一些加權方式。國際標準ITU-R BS.1770對於聲音響度定義LKFS則採用的是K-weighting filter(FIG. 5)，把聲音大致分三個權重區間：
>- <100Hz：越低頻越感受不到音量，所以測量值需減去越多
>- 100~1K Hz：重要性持平，維持原測量的音量值
>- ≥1KHz：放大高頻音量值

- [Audio Normalization 的二三事（下）](https://medium.com/kkstream/%E6%9C%89%E9%97%9Caudio-normalization%E5%85%A9%E4%B8%89%E4%BA%8B-%E4%B8%8B-c74f42ccc3f6)
>聲音訊號在數位系統的世界裡，採用的是dBFS (Decibels relative to full scale)為單位，以系統所能處理的最大量(Full scale)為基準，定為0 dBFS，並對其他值再做相對應的換算，所以數位音訊的db值均會是≤ 0。
>Audio normalization可以分為兩種做法：Peak (level) normalization 與 Loudness normalization。

>**Peak normalization**：將音訊最大(Peak)的地方，調整到特定音量，然後其他音訊做相對應的增/減調整。優點是方法簡單，只需要找出最大聲處與其音量，就能決定了整體的音量調整。缺點是Peak對於整體音訊不一定具有代表性：某個時間點出現大聲，並不代表整體都是大聲。
>**Loudness normalization**：先計算出整體聲音的平均音量，再將根據平均音量到期望音量的差值，作為調整的依據。好處是平均音量對於整體聲音比較具有代表性，缺點就是計算較於複雜，大音量的地方有可能在調整後，音量爆表而被裁切到或是各音量間的對比也會變得不一樣。
>![fig.1](https://i.imgur.com/QEUuVah.jpg)
>![](https://i.imgur.com/ch3QtTR.png)

- LKFS and LUFS relate to the same thing. Most of the world talks about LUFS, so we will too: but the US broadcast industry, particularly, likes to use LKFS.

- [Peak / Loudness Normalization](https://amvidia.com/guides/audio-conversion/peak-and-loudness-ebu-r128-normalization)
> In **Peak Normalization**, the analysis is straightforward and only considers how loud the Peak Level is. The gain is adjusted so that the maximum Peak Level equals some requested level or Target Level. An example is getting the highest volume of the audio. If you have a quiet sound, you may want to make it as loud possible, at Target Level 0 dBFS, without distortion or change its Dynamic Range. 
> ![](https://i.imgur.com/7sCk6Rb.png =450x)
> Another problem of Peak Normalization is the unpredictable Loudness Level as it does not directly correspond to Peak Level. Audios can be well normalized with Peak Normalization but may have very different Loudness Levels, so the volume needs to be often changed from song to song when listening. Loudness (EBU R128) Normalization is an excellent solution in such cases.

> In **Loudness Normalization**, perceived Loudness does not correspond to the Peak Level. Below are examples of two audios, normalized to -15 LUFS with Loudness Normalization. Both audios have the same Perceived Loudness, but quite different Peak Levels. 
> ![](https://i.imgur.com/w26zMy4.png)
> Choosing the right Target Level in such a case is important. High values of Target Level may lead to compression and result in the difference of Perceived Loudness of output audio tracks. <font color = D3482A>In normal circumstances, the recommended value for Target Level should be between -23 LUFS and -10 LUFS.</font>

> There are different types of Loudness:
> - Momentary Loudness, measurement time ~ 400 ms
> - Short Term Loudness, measurement time ~ 3 seconds
> - Integrated Loudness, the measurement time is infinite or at least the duration of a separate audio track

- [Demystifying The Confusion Around Loudness, Metering & Levels](https://ask.audio/articles/demystifying-the-confusion-around-loudness-metering-levels)

- LKFS by Python (pyloudnorm)
    - [Overview](https://www.christiansteinmetz.com/projects-blog/pyloudnorm)
    > The first stage of the algorithm is comprised of the application of <font color = D3482A>the “K” frequency weighting which is comprised of two IIR filters, a high shelving filter and a high pass filter</font>. There has been much confusion over the proper way to implement these filters since the specification only provides filter coefficients for 48 kHz implementations. This has led to some inconsistency among different implementations since nearly all implementations support operation at different sampling rates and therefore defined their own method of deriving coefficients for alternate sampling rates. <font color = D3482A>*Brecht De Man* recently published a [paper](https://www.aes.org/e-lib/browse.cfm?elib=19790) where he went through the arithmetic of deriving the proper filter definitions from the supplied coefficients</font>. He also provided a [Python implementation](https://github.com/BrechtDeMan/loudness.py) using this method. 
    
    > The next stage involves computing the mean square value for each channel in the signal and then each channel is weighted where surround channels have larger weights and the LFE channel is ignored. The weighted signals are then summed. <font color = D3482A>A gating process is then applied to this sum which attempts to mitigate the effect of portions of silence or near silence in the signal that would otherwise lower the measured integrated loudness of a signal. </font>
    
    > <font color = D3482A>Two thresholds are used in this gating process, where the signal is split up into 400 ms blocks with 75% overlap. </font>Each block is then measured against <font color = 239D55>the first threshold at -70 LKFS and the second at -10dB relative to the loudness of the signal once blocks below the -70 LKFS have been removed</font>. Finally the loudness measurement is based upon all gating blocks which contain a measurement above both the absolute and relative thresholds. 

    > It should be noted that while this algorithm has been shown to be effective for use on audio programmes that are typical of broadcast content, the algorithm is not, in general, suitable for use to estimate the subjective loudness of pure tones.

    > For that reason pyloudnorm has been built to allow easy programatic control of these settings. <font color = 239D55>Users can choose from filter weighting filters proposed</font> in the literature and even provide their own IIR filter specifications if desired. In addition <font color = 239D55>gating block sizes can easily be adjusted</font> to match recommendations from the literature and also extended. 
    - [Paper](https://csteinmetz1.github.io/pyloudnorm-eval/paper/pyloudnorm_preprint.pdf)
    > ![](https://i.imgur.com/GhsgEsi.png)
    > $z_{ij}=\frac{1}{N}\sum_{n=1}^{N}y_{ij}[n]^2$
    - [Github](https://github.com/csteinmetz1/pyloudnorm)

- K-filter coefficients ([loudness.py](https://github.com/BrechtDeMan/loudness.py))
> ![](https://i.imgur.com/MJMtPSf.png =400x)

> ![](https://i.imgur.com/Gtx0FcC.png =410x)

## Specs

- [Wikipedia](https://en.wikipedia.org/wiki/LKFS)
- [ITU-R BS.1770-4](https://www.itu.int/dms_pubrec/itu-r/rec/bs/R-REC-BS.1770-4-201510-I!!PDF-E.pdf) (fourth edition)

![](https://i.imgur.com/ckfvm7t.png)
![](https://i.imgur.com/07iRq0c.png =500x)![](https://i.imgur.com/3gBWPIh.png =500x)

### Formulea

| Symbol | Definition |
| :---: | :--- |
| $x$ | input signal |
| $y$ | k-filtered signal |
| $z$ | mean-square signal => power |
| $T$ | measurment interval |
| $I$ | $\{L,R,C,L_s,R_s\}$ |
| $G$ | weighted coeff for channel |
| $T_g$ | gating block interval |
| $L_k$ | loudness |
| $l$ | gating block loudness |
| $J_g$ | gating block loudness above threshold => $\{j:l_j>\Gamma\}$ |
| $\Gamma_a$ | absolute threshold, in spec -70 LKFS |
| $\Gamma_r$ | relative threshold |
| $L_{KG}$ | gated loudness |

Gated Loudness:
$$
L_{KG}=-0.691+10\log_{10}\sum_iG_i\cdot\left(\frac{1}{|J_g|}\cdot\sum_{J_g}z_{ij}\right) \ LKFS \\ where\ J_g=\{j:l_j>\Gamma_r\ and\ l_j>\Gamma_a\}
$$

The jth gating block loudness is defined as:
$$
l_j = -0.691 + 10\log_{10}\sum_{i}G_i\cdot z_{ij}
$$

The power, the mean square of the jth gating block:
$$
z_{ij}=\frac{1}{T_g}\ \int\limits_{T_g\cdot j\cdot step}^{T_g\cdot (j\cdot step+1)}y_i^2\ dt \\ where\ step = 1-overlap,\ j\in \left\{ 0,1,2,...,\frac{T-T_g}{T_g\cdot step} \right\}
$$

Relative Threshold:
$$
\Gamma_r=-0.691+10\log_{10}\sum_iG_i\cdot\left(\frac{1}{|J_g|}\cdot\sum_{J_g}z_{ij}\right)-10 \ LKFS \\ where\ J_g =\{j:l_j>\Gamma_a\},\ \Gamma_a = -70\ LKFS
$$

## Questions
- In fig.1 各類型音訊是怎麼分出來的? (Loudness, Headroom, Average Level, Noise floor)
- Live stream adjustment?
- [Learning Matplotlib, understanding diff approaches!](https://zhuanlan.zhihu.com/p/93423829)

## Original Hackmd :
https://hackmd.io/@jasonho610/HJa8jjVVu</center>

## Minute a Meeting
### < 2021/03/24 >
- ~~Peak v.s Loudness Normalization~~
- ~~Gated LKFS formulea~~
- Loudness, Headroom, Average Level, Noise floor
- fig.1 source

### < 2021/03/31 >
- ~~Unit~~
- ~~Integral / Summation~~
- ~~Trace the python code~~
- Why offset -0.691?

### < 2020/04/30 > 
- ~~Dependency~~
- ~~makefile~~

### < 2020/05/14 > 
- ~~Datatype conversion (double 0.0)~~
- ~~Normalize~~
- ~~argc, argv, -i/-o~~
- ~~Github~~
- Mel Scale

## Code note

```python=
K_filter(data, fs, True);
```
<font color=#31E360 size = 4>**<input.wav>**</font>

![](https://i.imgur.com/sjtqiu6.png =400x)
![](https://i.imgur.com/NIA5s8d.png =500x)
![](https://i.imgur.com/DR3xJhr.png =400x)
![](https://i.imgur.com/5ITFPjV.png =500x)

<font color=#31E360 size = 4>**<Justin Bieber - Peaches ft. Daniel Caesar, Giveon.wav>**</font>

![](https://i.imgur.com/x9hfy5c.png =400x)
![](https://i.imgur.com/QbA418c.png =500x)
![](https://i.imgur.com/nZldMkD.png =400x)
![](https://i.imgur.com/AAtYum7.png =500x)

