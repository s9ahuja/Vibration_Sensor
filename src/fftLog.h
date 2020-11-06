#ifndef FFTLog_h /* Prevent loading library twice */
#define FFTLog_h

#include "ArduinoFFT.h" // FFT header file
#include "constants.h"

enum Axis {
  x,
  y,
  z
};

struct IntTriplet {
  int x;
  int y;
  int z;
};

class FFTLog {
  private:
    static ArduinoFFT fftHelper;
    IntTriplet accSamples[SAMPLES] = {{0, 0, 0}};
    int fft[SAMPLES/2] = {0};
    int fftSize = SAMPLES/2;
    int xPin, yPin, zPin;
    int exponent;
    bool hasFFTBeenCalculated = false, hasDataBeenCollected = false;

  public:
    FFTLog() {
      this->xPin = A2;
      this->yPin = A1;
      this->zPin = A0;
    };
    FFTLog(int xPin, int yPin, int zPin);
    void readAcc(); // this is a blocking function (for SAMPLES/SAMPLING FREQ [s])
    void calcFFT(IntTriplet offsets);
    bool isFFTCalc();
    int getFFTFreqPow(int i);
    int getFFTArrSize();
    void reset();
    int getAxisAvg(Axis curAxis);
    void toString(char* buffer, int bufferSize);
};

#endif
