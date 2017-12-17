// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Please use an Arduino IDE 1.6.8 or greater

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>

#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <AzureIoTUtility.h>

#include "config.h"

static bool messagePending = false;
static bool messageSending = true;

static char *connectionString;
static char *ssid;
static char *pass;

static int interval = INTERVAL;

void blinkLED()
{
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
}

void initWifi()
{
    // Attempt to connect to Wifi network:
    Serial.printf("Attempting to connect to SSID: %s.\r\n", ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        // Get Mac Address and show it.
        // WiFi.macAddress(mac) save the mac address into a six length array, but the endian may be different. The huzzah board should
        // start from mac[0] to mac[5], but some other kinds of board run in the oppsite direction.
        uint8_t mac[6];
        WiFi.macAddress(mac);
        Serial.printf("You device with MAC address %02x:%02x:%02x:%02x:%02x:%02x connects to %s failed! Waiting 10 seconds to retry.\r\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ssid);
        WiFi.begin(ssid, pass);
        delay(10000);
    }
    Serial.printf("Connected to wifi %s.\r\n", ssid);
}

void initTime()
{
    time_t epochTime;
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    while (true)
    {
        epochTime = time(NULL);

        if (epochTime == 0)
        {
            Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
            delay(2000);
        }
        else
        {
            Serial.printf("Fetched NTP epoch time is: %lu.\r\n", epochTime);
            break;
        }
    }
}

static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
void setup()
{
    pinMode(LED_PIN, OUTPUT);

    initSerial();
    delay(2000);
    readCredentials();

    initWifi();
    initTime();
    initSensor();
    
    /*
    * Break changes in version 1.0.34: AzureIoTHub library removed AzureIoTClient class.
    * So we remove the code below to avoid compile error.
    */
    // initIoThubClient();

    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
    if (iotHubClientHandle == NULL)
    {
        Serial.println("Failed on IoTHubClient_CreateFromConnectionString.");
        while (1);
    }

    IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, receiveMessageCallback, NULL);
    IoTHubClient_LL_SetDeviceMethodCallback(iotHubClientHandle, deviceMethodCallback, NULL);
    IoTHubClient_LL_SetDeviceTwinCallback(iotHubClientHandle, twinCallback, NULL);


  // add for project
  pinMode(trigPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);
  pinMode(trigPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);

  // Initial state for states and timers..
  detection_state = NEW_DETECTION_STATE;
}

static int messageCount = 1;
bool enableLeftSensor = true;
bool enableRightSensor = true;

void loop()
{
//    if (!messagePending && messageSending)
//    {
//        char messagePayload[MESSAGE_MAX_LEN];
//        bool temperatureAlert = readMessage(messageCount, messagePayload);
//        sendMessage(iotHubClientHandle, messagePayload, temperatureAlert);
//        messageCount++;
//        //delay(interval);
//    }
//    IoTHubClient_LL_DoWork(iotHubClientHandle);
//    delay(10);





  // add for project
  long distanceLeft;
  long durationLeft;
  long distanceRight;
  long durationRight;
  
  unsigned long curTimeLeft = 0;
  unsigned long curTimeRight = 0;

  if (enableLeftSensor) {  
    digitalWrite(trigPinLeft, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPinLeft, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPinLeft, LOW);
    durationLeft = pulseIn(echoPinLeft, HIGH);
    distanceLeft = (durationLeft/2) / 29.1;
  }

  if (enableRightSensor) {  
    digitalWrite(trigPinRight, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPinRight, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPinRight, LOW);
    durationRight = pulseIn(echoPinRight, HIGH);
    distanceRight = (durationRight/2) / 29.1;
  }
  

  // sensor state machine.
  switch (detection_state) {
    
    case NEW_DETECTION_STATE:
    // new detection state
    if ( distanceLeft < detectHumanDistance ) {
      // approch left sensor
      detection_state = APPROCH_LEFT_FROM_NEW_STATE;

      enableLeftSensor = false;
      
    } else if ( distanceRight < detectHumanDistance ) {
      // approch right sensor
      detection_state = APPROCH_RIGHT_FROM_NEW_STATE;

      enableRightSensor = false;
    }
    break;
    
    case APPROCH_LEFT_FROM_NEW_STATE:
    if ( distanceRight < detectHumanDistance ) { // approch right sensor
      //
      detection_state = APPROCH_RIGHT_FROM_LEFT_STATE;
    }
    break;
    
    case APPROCH_RIGHT_FROM_LEFT_STATE:
    if ( distanceRight > detectNoHumanDistance ) { // leaving right sensor
      //
      detection_state = NEW_DETECTION_STATE;

      enableLeftSensor = true;
      
      peopleNum++ ;

      
//      if ( peopleNum%3 == 0 ) {
//          if (!messagePending && messageSending)
//          {
//              char messagePayload[MESSAGE_MAX_LEN];
//              bool temperatureAlert = readMessage(messageCount, messagePayload);
//              sendMessage(iotHubClientHandle, messagePayload, temperatureAlert);
//              messageCount++;
//              //delay(interval);
//          }
//          IoTHubClient_LL_DoWork(iotHubClientHandle);
//          delay(10);
//      }

      if (!messagePending && messageSending)
      {
          char messagePayload[MESSAGE_MAX_LEN];
          bool temperatureAlert = readMessage(messageCount, messagePayload);
          sendMessage(iotHubClientHandle, messagePayload, temperatureAlert);
          messageCount++;
          //delay(interval);
      }
      IoTHubClient_LL_DoWork(iotHubClientHandle);
      delay(10);


    }
    break;
    
    case APPROCH_RIGHT_FROM_NEW_STATE:
    if ( distanceLeft < detectHumanDistance ) { // approch left sensor
      //
      detection_state = APPROCH_LEFT_FROM_RIGHT_STATE;
    }
    break;

    case APPROCH_LEFT_FROM_RIGHT_STATE:
    if ( distanceLeft > detectNoHumanDistance ) { // leaving left sensor
      //
      detection_state = NEW_DETECTION_STATE;

      enableRightSensor = true;
      
      peopleNum-- ;

//      if ( peopleNum%3 == 0 ) {
//          if (!messagePending && messageSending)
//          {
//              char messagePayload[MESSAGE_MAX_LEN];
//              bool temperatureAlert = readMessage(messageCount, messagePayload);
//              sendMessage(iotHubClientHandle, messagePayload, temperatureAlert);
//              messageCount++;
//              //delay(interval);
//          }
//          IoTHubClient_LL_DoWork(iotHubClientHandle);
//          delay(10);
//      }

      if (!messagePending && messageSending)
      {
          char messagePayload[MESSAGE_MAX_LEN];
          bool temperatureAlert = readMessage(messageCount, messagePayload);
          sendMessage(iotHubClientHandle, messagePayload, temperatureAlert);
          messageCount++;
          //delay(interval);
      }
      IoTHubClient_LL_DoWork(iotHubClientHandle);
      delay(10);
      
    }
    break;
  }


  


/*

  if((oldDistanceLeft - distanceLeft) < -20 ){
    while(((oldDistanceRight - distanceRight) > -20) && ((oldDistanceLeft - distanceLeft) > -20)){
      Serial.println("people in btw sensors");
    }
    peopleNum ++ ;
  }

  if ((oldDistanceRight - distanceRight) < -20){
    while(((oldDistanceRight - distanceRight) > -20) && ((oldDistanceLeft - distanceLeft) > -20)){
      Serial.println("people in btw sensors");
    }
    peopleNum -- ;
  }

  
  
  oldDistanceLeft = distanceLeft;
  oldDistanceRight = distanceRight;

  */
/*
  Serial.print("distanceLeft: ");
  Serial.print(distanceLeft);
  Serial.print(" cm");
  Serial.print("distanceRight: ");
  Serial.print(distanceRight);
  Serial.println(" cm");

  Serial.print("peopleNum: ");
  Serial.println(peopleNum);

*/

  //delay(50);
    
}
