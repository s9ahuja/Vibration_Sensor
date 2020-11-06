#ifndef RSSI_H
#define RSSI_H

#include "Particle.h"
#include "Arduino.h"
#include <CellularHelper.h>
#include "constants.h"


class RSSI {
  private:
    static char requestBuf[256];
    static char *requestCur;
    static int numAdded;
    static String eventName;
  	String oper = "AT&T"; // Used for LTE (SARA-R410M-02B only)
  	int mcc = 310; // LTE
  	int mnc = 410; // LTE

    void cellularAddTower(CellularHelperEnvironmentCellData *cellData);
    char *cellularScan();
    char *cellularScanLTE();
    char *scan();

  public:
  	void publishLocation();
};

#endif
