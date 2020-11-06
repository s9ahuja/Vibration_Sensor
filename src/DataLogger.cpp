#include "DataLogger.h"

DataLogger::DataLogger(int xPin, int yPin, int zPin){
  // for (int i = 0; i < 750; i++){
  //   this->workingDataBuffer = FFTLog(pinX, pinY, pinZ);
  // }
}

void DataLogger::serialPrintData(){
    for (int i = 0; i < CACHE_SIZE; i++){
    String tsString = "{";
      for (int j = 0; j < SAMPLES/2; j++) {
        // tsString += String(this->dataCache[i].data[j].min);
        // tsString += ":";
        // tsString += String(this->dataCache[i].data[j].firstQuart);
        // tsString += ":";
        tsString += String(this->dataCache[i].data[j].median);
        // tsString += ":";
        // tsString += String(this->dataCache[i].data[j].thirdQuart);
        // tsString += ":";
        // tsString += String(this->dataCache[i].data[j].max);
        // tsString += ", ";
      }
      tsString += "}";
      Serial.println(tsString);
    }
}

void DataLogger::runSample(){
  #ifdef DEBUG_MODE
  Serial.println("Sampling...");
  #endif
  for (int i = 0; i < FFT_BUFFER_SIZE; i++){
    this->workingDataBuffer[i].readAcc();
  }

  #ifdef DEBUG_MODE
  Serial.println("Calculating Gravity Offset...");
  #endif
  IntTriplet offsets = {0,0,0};
  for (int i = 0; i < FFT_BUFFER_SIZE; i++){
    offsets.x += this->workingDataBuffer[i].getAxisAvg(x);
    offsets.y += this->workingDataBuffer[i].getAxisAvg(y);
    offsets.z += this->workingDataBuffer[i].getAxisAvg(z);
  }
  offsets.x /= FFT_BUFFER_SIZE;
  offsets.y /= FFT_BUFFER_SIZE;
  offsets.z /= FFT_BUFFER_SIZE;

  // Serial.println("Calculating FFTs...");
  for (int i = 0; i < FFT_BUFFER_SIZE; i++){
    this->workingDataBuffer[i].calcFFT(offsets);
  }

  // aggregate stats and insert into dataCache
  #ifdef DEBUG_MODE
  Serial.println("Calculating Aggregate Stats...");
  #endif
  for (int freqIndex = 0; freqIndex < SAMPLES/2; freqIndex++){ // loop through fft frequency
    int sortingSamples[FFT_BUFFER_SIZE];
    // loop through all samples (load into sortingSamples)
    for (int i = 0; i < FFT_BUFFER_SIZE; i++){
      sortingSamples[i] = this->workingDataBuffer[i].getFFTFreqPow(freqIndex);
      Serial.println(sortingSamples[i]);
    }

    // sort data
    int sCount; //sortedCount
    int cCount; //comparedCount

    for (sCount = 0; sCount < FFT_BUFFER_SIZE - 1; sCount++)
    {
      int minVal = sCount;
      for(cCount = sCount + 1; cCount < FFT_BUFFER_SIZE; cCount++)
      {
        if(sortingSamples[cCount] < sortingSamples[minVal])
        {
          minVal = cCount;
        }
      }
      if (minVal != sCount)
      {
        int swap = 0;
        swap = sortingSamples[minVal];
        sortingSamples[minVal] = sortingSamples[sCount];
        sortingSamples[sCount] = swap;
      }
    }

    // load rows into summary
    this->dataCache[this->dataCacheEndIdx].timestamp = Time.now();
    // this->dataCache[this->dataCacheEndIdx].data[freqIndex].min=sortingSamples[0*(FFT_BUFFER_SIZE-1)/4];
    // this->dataCache[this->dataCacheEndIdx].data[freqIndex].firstQuart=sortingSamples[1*(FFT_BUFFER_SIZE-1)/4];
    this->dataCache[this->dataCacheEndIdx].data[freqIndex].median=sortingSamples[2*(FFT_BUFFER_SIZE-1)/4];
    // this->dataCache[this->dataCacheEndIdx].data[freqIndex].thirdQuart=sortingSamples[3*(FFT_BUFFER_SIZE-1)/4];
    // this->dataCache[this->dataCacheEndIdx].data[freqIndex].max=sortingSamples[4*(FFT_BUFFER_SIZE-1)/4];
  }
  // Serial.println(this->dataCache[this->dataCacheEndIdx].toString());
  if (++this->dataCacheEndIdx == CACHE_SIZE)
  {
    this->dataCacheEndIdx = 0;
    isCacheFull = TRUE;
  }
}


void DataLogger::publishData(){
  if(isCacheFull) {
    dataCacheEndIdx = CACHE_SIZE;
  }

  for (int i = 0; i < dataCacheEndIdx; i++) {
    String outString = this->dataCache[i].toString();
    if (Cellular.ready() && Particle.connected()){
      Particle.publish("fft", outString, PRIVATE);
      unsigned long uploadTimer = millis();
      while (millis() - uploadTimer < 1000) {
        Particle.process();
      }
    }

    #ifdef DEBUG_MODE
    Serial.println("Printing Cache: Line " + String(i) + "...");
    Serial.println(outString);
    #endif
  }
}

void DataLogger::resetCache(){
  if(isCacheFull) {
    dataCacheEndIdx = CACHE_SIZE;
  }

  for (int i = 0; i < dataCacheEndIdx; i++) {
    for (int j = 0; j < SAMPLES/2; j++) {
      // this->dataCache[i].data[j].min = -1;
      // this->dataCache[i].data[j].firstQuart = -1;
      this->dataCache[i].data[j].median = -1;
      // this->dataCache[i].data[j].thirdQuart = -1;
      // this->dataCache[i].data[j].max = -1;
    }
  }
  dataCacheEndIdx = 0;
  isCacheFull = false;
}
