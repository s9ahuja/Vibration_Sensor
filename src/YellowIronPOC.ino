/*
* Project: Yellow Iron POC
* Description: An IoT device to be put in construction machinery that transmits GPS location and frequency profile of the observed vibrations (to determine machinery state: OFF, IDLE, IN-USE)
* Author: Kaspar Shahzada
* Date: Feb 13th, 2019
*/

PRODUCT_ID(8460);
PRODUCT_VERSION(2);

#include "RSSI.h"
#include "DataLogger.h"
#include "constants.h"

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

DataLogger dataLogger = DataLogger(ACC_X_PIN,ACC_Y_PIN,ACC_Z_PIN);
RSSI locationService = RSSI();

unsigned long lastDataRecording = 0;
unsigned long lastDataUpload = 0;

bool doingUpdate = false;

void updateHandler(){
  Serial.println("Update Available");
  digitalWrite(BOARD_LED_PIN, HIGH);
  doingUpdate = true;
}

void setup() {
  System.on(firmware_update+firmware_update_pending, updateHandler);
  pinMode(BOARD_LED_PIN, OUTPUT);
  pinMode(ACC_X_PIN, INPUT);
  pinMode(ACC_Y_PIN, INPUT);
  pinMode(ACC_Z_PIN, INPUT);

  #ifdef DEBUG_MODE
  Serial.begin(9600);
  Serial.println("Running in debug mode...");
  #endif

  #ifndef OFFLINE_MODE
  // cycle on cloud to get heartbeat
  Cellular.on();
  Particle.connect();
  waitFor(Particle.connected, 30000);
  unsigned long uploadTimer = millis();
  while (millis() - uploadTimer < 15000) {
    Particle.process();
  }
  Serial.println("Connected init");
  #endif

  for (int i = 0; i < 10; i++){
    digitalWrite(BOARD_LED_PIN, HIGH);
    delay(200);
    digitalWrite(BOARD_LED_PIN, LOW);
    delay(200);
  }

  lastDataUpload = Time.now();
  lastDataRecording = Time.now();
}

void loop()
{
  if(!doingUpdate){
    Serial.println("No update.");
    // Print State if in Debug
    if(Time.now() - lastDataRecording > FFT_PERIOD){
      lastDataRecording = Time.now();
      dataLogger.runSample();
      Serial.println("Running sample.");
    }

    if(Time.now() - lastDataUpload > UPLOAD_PERIOD){
      lastDataUpload = Time.now();
      #ifdef DEBUG_MODE
      Serial.println("Uploading...");
      #endif
      #ifndef OFFLINE_MODE
      Cellular.on();
      Particle.connect();
      waitFor(Particle.connected, 30000);
      // while (millis() - uploadTimer > 15000) {
      //   Particle.process();
      // }
      // uploadTimer = millis();
      Serial.println("Connected loop.");
      Serial.println(Particle.connected());
      if(Particle.connected()){
        locationService.publishLocation();
        Serial.println("Loc published.");
        dataLogger.publishData();
        Serial.println("Data published.");
        dataLogger.resetCache();
        Serial.println("Cache clear.");
      }
      #else
      locationService.publishLocation();
      Serial.println("Loc published.");
      dataLogger.publishData();
      Serial.println("Data published.");
      dataLogger.resetCache();
      Serial.println("Cache clear.");
      #endif
    }
    if(!doingUpdate){
      Serial.println("No update--");
      Cellular.off();
      System.sleep(D1, RISING, SLEEP_NETWORK_STANDBY, SLEEP_PERIOD);
    }
  } else {
    Serial.println("Waiting for update...");
    Cellular.on();
    Particle.connect();
    waitFor(Particle.connected, 30000);
    Serial.println("Connected");
      while(true){};
  }
}
