#include "fftLog.h"

ArduinoFFT FFTLog::fftHelper = ArduinoFFT();

// initialize pins for accelermeters
FFTLog::FFTLog(int xPin, int yPin, int zPin){
  this->exponent = this->fftHelper.Exponent(SAMPLES);
  this->xPin = xPin;
  this->yPin = yPin;
  this->zPin = zPin;
}

// fill out the recorded accelerometer data
void FFTLog::readAcc(){
  unsigned long loopTimer = micros();
  for (int i = 0; i < SAMPLES; i++)
  {
    while (micros() - loopTimer < 1000*1000/SAMPLING_FREQ);
    loopTimer = micros();
    this->accSamples[i].x = analogRead(this->xPin);
    this->accSamples[i].y = analogRead(this->yPin);
    this->accSamples[i].z = analogRead(this->zPin);
    // Serial.println(this->accSamples[i].x);
    // Serial.println(":");
    // Serial.println(this->accSamples[i].y);
    // Serial.println(":");
    // Serial.println(this->accSamples[i].z);
    // Serial.println(", ");
  }
  this->hasDataBeenCollected = true;
}

void FFTLog::calcFFT(IntTriplet offsets){
  if(!this->hasDataBeenCollected){
    return;
  }

  double vReal[SAMPLES] = {0};
  double vImag[SAMPLES] = {0};

  for (int i = 0; i < SAMPLES; i++)
  {
    int x = this->accSamples[i].x - offsets.x;
    int y = this->accSamples[i].y - offsets.y;
    int z = this->accSamples[i].z - offsets.z;
    vReal[i] = sqrt(x*x + y*y + z*z);
  }

  fftHelper.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
  fftHelper.Compute(vReal, vImag, SAMPLES, exponent, FFT_FORWARD); /* Compute FFT */
  fftHelper.ComplexToMagnitude(vReal, vImag, SAMPLES); /* Compute magnitudes */

  for (int i = 0; i < SAMPLES/2; i++)
  {
    this->fft[i] = vReal[i];
    // Serial.println(String(vReal[i]));
  }
  this->hasFFTBeenCalculated = true;
}

bool FFTLog::isFFTCalc(){
  return this->hasFFTBeenCalculated;
}

int FFTLog::getFFTFreqPow(int i){
  return this->fft[i];
}

int FFTLog::getFFTArrSize(){
  return this->fftSize;
}

void FFTLog::reset(){
  this->hasDataBeenCollected = false;
  this->hasFFTBeenCalculated = false;
}

int FFTLog::getAxisAvg(Axis curAxis){
  if (!this->hasDataBeenCollected){
    return -1;
  }

  long avg = 0;
  for (int i = 0; i < SAMPLES; i++) {
    IntTriplet curAcc = this->accSamples[i];
    switch(curAxis){
      case x:
        avg += curAcc.x;
        break;
      case y:
        avg += curAcc.y;
        break;
      case z:
        avg += curAcc.z;
        break;
    }
  }
  avg /= SAMPLES;
  // Serial.println(String(avg));
  return avg;
}

void FFTLog::toString(char* buffer, int bufferSize){
  sprintf(buffer, "Hello World");
  return;
}

// String toString() {
//   FFT_String1 = "{";
//   for (int i = 0; i < SAMPLES/2; i++) {
//     FFT_String1 += String(sampleList[sampleNum][i]);
//   }
//   FFT_String1 += "}";
//   return FFT_String1;
// }
