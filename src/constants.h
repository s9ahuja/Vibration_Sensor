#ifndef constants_h /* Prevent loading library twice */
#define constants_h

#define FFT_PERIOD 15 // [s]
#define UPLOAD_PERIOD 900 // [s]
#define SLEEP_PERIOD 15 // [s]
// #define UPLOAD_PERIOD 10800000

// #define DEBUG_MODE // comment out for release
// #define OFFLINE_MODE // comment out for release

// Pin Definitions
#define BOARD_LED_PIN D7
#define ACC_X_PIN A2
#define ACC_Y_PIN A1
#define ACC_Z_PIN A0

#define FFT_BUFFER_SIZE 12 // approximately a 3.8s sample time
// #define CACHE_SIZE 2880 // commenting out because it increases the compile time
#define CACHE_SIZE 750

#define ACC_X_PIN A2
#define ACC_Y_PIN A1
#define ACC_Z_PIN A0

#define SAMPLES 64 // this must be a 2^N
#define SAMPLING_FREQ 200 // sampling frequency [Hz]

#endif
