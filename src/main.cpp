#include <Arduino.h>
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "https://xxxxxxxx.firebaseio.com"
#define FIREBASE_AUTH "database secrect"
#define WIFI_SSID "your wifi name"
#define WIFI_PASSWORD "your password"

const int relay1 = 0; //GPIO 0      Air Conditioner
const int relay2 = 2; //GPIO 2      Ground
const int relay3 = 1; //GPIO 1 (TX) Light
const int relay4 = 3; //GPIO 3 (RX) Fan

//FirebaseObject esp01;

const String path = "/devices/dec_1/";

//Define FirebaseESP8266 data object for data sending and receiving
FirebaseData firebaseData;

void switch_to_GPIO(int pin1, int pin3)
{

    pinMode(pin1, FUNCTION_3); //GPIO 1 (TX) swap the pin to a GPIO.
    pinMode(pin3, FUNCTION_3); //GPIO 3 (RX) swap the pin to a GPIO.

    pinMode(pin1, OUTPUT);
    pinMode(pin3, OUTPUT);

    Serial.end();
}

void switch_to_UART(int pin1, int pin3)
{
    pinMode(pin1, FUNCTION_0); //GPIO 1 swap the pin to a UART_TX.
    pinMode(pin3, FUNCTION_0); //GPIO 3 swap the pin to a UART_RX.

    Serial.begin(115200);
}

void setData(int status)
{
    if (Firebase.setInt(firebaseData, path + "/isLightOn", status))
    {
        //Success
        Serial.print("Set int data success, int = ");
        Serial.println(firebaseData.intData());
        delay(200);
    }
    else
    {
        //Failed?, get the error reason from firebaseData
        Serial.print("Error in setInt, ");
        Serial.println(firebaseData.errorReason());
        delay(200);
    }
}

void getData()
{
    //Try to get int data from Firebase
    if (Firebase.getInt(firebaseData, path + "/isLightOn"))
    {
        //Success
        Serial.print("Get int data success, int = ");
        Serial.println(firebaseData.intData());
        if (firebaseData.intData() == 1)
        {
            digitalWrite(relay1, HIGH);
        }
        else
        {
            digitalWrite(relay1, LOW);
        }
    }
    else
    {
        //Failed?, get the error reason from firebaseData
        Serial.print("Error in getInt, ");
        Serial.println(firebaseData.errorReason());
    }
}

void setup()
{

    pinMode(relay1, OUTPUT);
    pinMode(relay2, OUTPUT);
    switch_to_UART(relay3, relay4);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("*");
        delay(1000);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    //Set Firebase info
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

    //Enable auto reconnect the WiFi when connection lost
    Firebase.reconnectWiFi(true);
}

void loop()
{
    setData(1);
    getData();
    delay(1000);
    setData(0);
    getData();
    delay(1000);
}