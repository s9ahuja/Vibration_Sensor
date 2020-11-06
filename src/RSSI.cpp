#include "Particle.h"
#include "RSSI.h"

char RSSI::requestBuf[256];
char *RSSI::requestCur;
int RSSI::numAdded = 0;
String RSSI::eventName="rssi";

char *RSSI::scan() {
	return cellularScan();
}

void RSSI::publishLocation() {
	const char *scanData = scan();
	if (scanData[0]) {
		if (Particle.connected()) {
			Particle.publish(eventName, scanData, PRIVATE);
		}
	}
	#ifdef DEBUG
	Serial.println("Publishing Location...");
	Serial.printlnf("scanData:%s", scanData);
	#endif
}

void RSSI::cellularAddTower(CellularHelperEnvironmentCellData *cellData) {
	// The - 4 factor here to leave room for the closing JSON array ], object }}, and the trailing null
	size_t spaceLeft = &requestBuf[sizeof(requestBuf) - 4] - requestCur;

	size_t sizeNeeded = snprintf(requestCur, spaceLeft,
			"%s{\"i\":%d,\"l\":%u,\"c\":%d,\"n\":%d}",
			(requestCur[-1] == '[' ? "" : ","),
			cellData->ci, cellData->lac, cellData->mcc, cellData->mnc);

	if (sizeNeeded <= spaceLeft && cellData->lac != 0 && cellData->lac != 65535 && cellData->mcc != 65535 && cellData->mnc != 65535) {
		// There is enough space to store the whole entry, so save it
		requestCur += sizeNeeded;
		numAdded++;
	}
}


char *RSSI::cellularScan() {
	requestCur = requestBuf;
	numAdded = 0;

	if (CellularHelper.isLTE()) {
		return cellularScanLTE();
	}

	// First try to get info on neighboring cells. This doesn't work for me using the U260
	CellularHelperEnvironmentResponseStatic<4> envResp;

	CellularHelper.getEnvironment(5, envResp);

	if (envResp.resp != RESP_OK) {
		// We couldn't get neighboring cells, so try just the receiving cell
		CellularHelper.getEnvironment(3, envResp);
	}
	// envResp.serialDebug();


	// We know these things fit, so just using sprintf instead of snprintf here
	requestCur += sprintf(requestCur, "{\"c\":{\"o\":\"%s\",",
			CellularHelper.getOperatorName().c_str());

	requestCur += sprintf(requestCur, "\"a\":[");

	cellularAddTower(&envResp.service);

	for(size_t ii = 0; ii < envResp.getNumNeighbors(); ii++) {
		cellularAddTower(&envResp.neighbors[ii]);
	}

	*requestCur++ = ']';
	*requestCur++ = '}';
	*requestCur++ = '}';
	*requestCur++ = 0;

	if (numAdded == 0) {
		requestBuf[0] = 0;
	}

	return requestBuf;
}

char *RSSI::cellularScanLTE() {

	CellularHelperCREGResponse resp;
	CellularHelper.getCREG(resp);

	// Serial.println(resp.toString().c_str());

	// We know these things fit, so just using sprintf instead of snprintf here
	requestCur += sprintf(requestCur, "{\"c\":{\"o\":\"%s\",", oper.c_str());

	requestCur += sprintf(requestCur, "\"a\":[");

	if (resp.valid) {
		requestCur += sprintf(requestCur,
					"{\"i\":%d,\"l\":%u,\"c\":%d,\"n\":%d}",
					resp.ci, resp.lac, mcc, mnc);

		numAdded++;
	}

	*requestCur++ = ']';
	*requestCur++ = '}';
	*requestCur++ = '}';
	*requestCur++ = 0;



	if (numAdded == 0) {
		requestBuf[0] = 0;
	}

	return requestBuf;
}
