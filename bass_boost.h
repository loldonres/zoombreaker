#pragma once
#include <cstdint>
#include <vector>

class BassBoost {
public:
    void SetSampleRate(int sampleRate);
    void SetAmount(float amount);
    void Reset();
    void Process(int16_t* data, size_t samples, int channels);

private:
    void RecomputeCoeffs();

    int   sampleRate_ = 44100;
    float amount_     = 0.0f;

     
    float b0_ = 1, b1_ = 0, b2_ = 0, a1_ = 0, a2_ = 0;
    float z1L_ = 0, z2L_ = 0;
    float z1R_ = 0, z2R_ = 0;

     
    float envL_ = 0.0f;
    float envR_ = 0.0f;

     
    int   zeroCrossL_ = 0;
    int   zeroCrossR_ = 0;
    float phaseL_ = 0.0f;
    float phaseR_ = 0.0f;
    int   lastSignL_ = 0;
    int   lastSignR_ = 0;
};