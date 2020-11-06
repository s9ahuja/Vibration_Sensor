#ifndef DataLogger_h /* Prevent loading library twice */
#define DataLogger_h

#include "fftLog.h" // FFT header file
#include "Arduino.h"
#include "constants.h"

struct DataStats {
  // int min;
  // int firstQuart;
  uint16_t median;
  // int thirdQuart;
  // int max;
  void serialPrint(){
    String tsString = "";
    // tsString += String(this->min);
    // tsString += ":";
    // tsString += String(this->firstQuart);
    // tsString += ":";
    tsString += String(this->median);
    // tsString += ":";
    // tsString += String(this->thirdQuart);
    // tsString += ":";
    // tsString += String(this->max);
    // tsString += ", ";
    Serial.println(tsString);
  };
};

struct DataRow {
  int timestamp = -1;
  // DataStats data[SAMPLES/2] = {{-1, -1, -1, -1, -1}}; // stats for each frequency in the fft
  DataStats data[SAMPLES/2] = {{0}};
  IntTriplet offsets = {-1, -1, -1};
  String toString(){
    String me = "{";

    me += "\"t\":" + String(timestamp);

    me += ",\"f\":" + String(SAMPLING_FREQ);

    me += ",\"d\":[";
    me += String((int)this->data[0].median);
    for (int i = 1; i < SAMPLES/2; i++){
      me += "," + String((int)this->data[i].median);
    }
    me += "]";

    me += "}";
    Serial.println(me.length());
    return me;
  };
};

class DataLogger {
  private:
    FFTLog workingDataBuffer[FFT_BUFFER_SIZE];

  public:
    DataRow dataCache[CACHE_SIZE];
    String tsString;
    int dataCacheEndIdx = 0;
    bool isCacheFull = false;
    DataLogger(int xPin, int yPin, int zPin);
    void serialPrintData();
    void runSample(); // runs a sample and stores it in the cache
    void publishData();
    void resetCache();
};

#endif
