#include "bass_boost.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void BassBoost::SetSampleRate(int sampleRate) {
    sampleRate_ = sampleRate > 0 ? sampleRate : 44100;
    RecomputeCoeffs();
}

void BassBoost::SetAmount(float amount) {
    amount_ = amount;
}

void BassBoost::Reset() {
    z1L_ = z2L_ = z1R_ = z2R_ = 0.0f;
    envL_ = envR_ = 0.0f;
    zeroCrossL_ = 0;
    zeroCrossR_ = 0;
    phaseL_ = 0.0f;
    phaseR_ = 0.0f;
    lastSignL_ = 0;
    lastSignR_ = 0;
}

void BassBoost::RecomputeCoeffs() {
     
    const float freq = 100.0f;
    const float widthHz = 60.0f;
    const float gainDb = 18.0f;

    const float A = powf(10.0f, gainDb / 40.0f);
    const float omega = 2.0f * (float)M_PI * freq / sampleRate_;
    const float sn = sinf(omega);
    const float cs = cosf(omega);
    const float alpha = sn * sinhf((float)M_PI * widthHz / sampleRate_);

    const float b0 = 1.0f + alpha * A;
    const float b1 = -2.0f * cs;
    const float b2 = 1.0f - alpha * A;
    const float a0 = 1.0f + alpha / A;
    const float a1 = -2.0f * cs;
    const float a2 = 1.0f - alpha / A;

    b0_ = b0 / a0;
    b1_ = b1 / a0;
    b2_ = b2 / a0;
    a1_ = a1 / a0;
    a2_ = a2 / a0;
}

 
static inline float Shape(float x, float drive) {
     
    const float ax = fabsf(x);
    const float shaped = 1.0f - expf(-ax * drive);
    return (x < 0.0f ? -shaped : shaped);
}

void BassBoost::Process(int16_t* data, size_t samples, int channels) {
    if (amount_ <= 0.0f || samples == 0 || channels < 1)
        return;

     
     
     
     
    const float boost = 1.0f + amount_ * 5.0f;

     
    const float drive = 3.0f + amount_ * 4.0f;

     
     
    const float zeroThreshold = 0.05f;

    for (size_t i = 0; i < samples; ++i) {
        if (channels >= 2) {
            float xL = (float)data[i * 2 + 0] / 32768.0f;
            float xR = (float)data[i * 2 + 1] / 32768.0f;

             
            float yL = b0_ * xL + z1L_;
            z1L_ = b1_ * xL - a1_ * yL + z2L_;
            z2L_ = b2_ * xL - a2_ * yL;

            float yR = b0_ * xR + z1R_;
            z1R_ = b1_ * xR - a1_ * yR + z2R_;
            z2R_ = b2_ * xR - a2_ * yR;

             
            yL *= boost;
            yR *= boost;

             
            yL = Shape(yL, drive);
            yR = Shape(yR, drive);

             
             
             
            float absL = fabsf(yL);
            float absR = fabsf(yR);

             
            int signL = (yL >= 0) ? 1 : -1;
            int signR = (yR >= 0) ? 1 : -1;

            if (signL != lastSignL_ && absL > zeroThreshold) {
                zeroCrossL_++;
                lastSignL_ = signL;
            }
            if (signR != lastSignR_ && absR > zeroThreshold) {
                zeroCrossR_++;
                lastSignR_ = signR;
            }

             
            if ((i & 31) == 0) {
                 
                 
                zeroCrossL_ = 0;
                zeroCrossR_ = 0;
            }

             
            phaseL_ += 0.5f * 0.01f;  
            phaseR_ += 0.5f * 0.01f;
            if (phaseL_ > 1.0f) phaseL_ -= 1.0f;
            if (phaseR_ > 1.0f) phaseR_ -= 1.0f;

             
            float subL = sinf(phaseL_ * 2.0f * (float)M_PI) * absL * 0.5f;
            float subR = sinf(phaseR_ * 2.0f * (float)M_PI) * absR * 0.5f;

            yL += subL * amount_;
            yR += subR * amount_;

             
            if (yL >  1.0f) yL =  1.0f;
            if (yL < -1.0f) yL = -1.0f;
            if (yR >  1.0f) yR =  1.0f;
            if (yR < -1.0f) yR = -1.0f;

            data[i * 2 + 0] = (int16_t)(yL * 32767.0f);
            data[i * 2 + 1] = (int16_t)(yR * 32767.0f);
        } else {
            float x = (float)data[i] / 32768.0f;

            float y = b0_ * x + z1L_;
            z1L_ = b1_ * x - a1_ * y + z2L_;
            z2L_ = b2_ * x - a2_ * y;

            y *= boost;
            y = Shape(y, drive);

            float absY = fabsf(y);
            int sign = (y >= 0) ? 1 : -1;
            if (sign != lastSignL_ && absY > zeroThreshold) {
                zeroCrossL_++;
                lastSignL_ = sign;
            }
            if ((i & 31) == 0) zeroCrossL_ = 0;

            phaseL_ += 0.5f * 0.01f;
            if (phaseL_ > 1.0f) phaseL_ -= 1.0f;

            float sub = sinf(phaseL_ * 2.0f * (float)M_PI) * absY * 0.5f;
            y += sub * amount_;

            if (y >  1.0f) y =  1.0f;
            if (y < -1.0f) y = -1.0f;

            data[i] = (int16_t)(y * 32767.0f);
        }
    }
}