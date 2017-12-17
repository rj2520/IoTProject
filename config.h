// Physical device information for board and sensor
#define DEVICE_ID "SparkFun ESP8266 Thing Dev"
#define DHT_TYPE DHT22

// Pin layout configuration
#define LED_PIN 5
#define DHT_PIN 2

#define TEMPERATURE_ALERT 30

// Interval time(ms) for sending message to IoT Hub
#define INTERVAL 20
//#define INTERVAL 2000

// If don't have a physical DHT sensor, can send simulated data to IoT hub
#define SIMULATED_DATA true
//#define SIMULATED_DATA false

// EEPROM address configuration
#define EEPROM_SIZE 512

// SSID and SSID password's length should < 32 bytes
// http://serverfault.com/a/45509
#define SSID_LEN 32
#define PASS_LEN 32
#define CONNECTION_STRING_LEN 256

#define MESSAGE_MAX_LEN 256



// add for project
#define trigPinLeft 13
#define echoPinLeft 12

#define trigPinRight 16
#define echoPinRight 15

long oldDistanceLeft;
long oldDistanceRight;

int peopleNum = 0;

#define NEW_DETECTION_STATE 1
#define APPROCH_LEFT_FROM_NEW_STATE 11
#define APPROCH_RIGHT_FROM_LEFT_STATE 12
#define APPROCH_RIGHT_FROM_NEW_STATE 21
#define APPROCH_LEFT_FROM_RIGHT_STATE 22

int detection_state; // detection state.

#define detectHumanDistance 20
#define detectNoHumanDistance 20


