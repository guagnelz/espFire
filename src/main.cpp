#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define FIREBASE_HOST "project.firebaseio.com"  //Change to your Firebase RTDB project ID e.g. Your_Project_ID.firebaseio.com
#define FIREBASE_AUTH "key" //Change to your Firebase RTDB secret password
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "passkey"

//Define Firebase Data objects
FirebaseData firebaseData;

const int relePin = 12; //GPIO5 or D2 for Rele // GPIO12 Sonoff
const int ledPin = 13; //GPIO5 or D2 for Rele // GPIO12 Sonoff
String path = "/Espompa";
String nodeID = "Rele";      //This is this node ID to receive control


void setup()
{

    Serial.begin(9600);

    pinMode(ledPin, OUTPUT);
    pinMode(relePin, OUTPUT);
    digitalWrite(ledPin, HIGH);

    Serial.print("ciao");
    Serial.println();

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        digitalWrite(ledPin, HIGH); // LOW will turn on the LED
        delay(500);
        digitalWrite(ledPin, LOW); // LOW will turn on the LED
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

    if (!Firebase.beginStream(firebaseData, path + "/" + nodeID))
    {
        Serial.println("Could not begin stream");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println();
    }
}

void loop()
{
    if (!Firebase.readStream(firebaseData))
    {
        Serial.println();
        Serial.println("Can't read stream data");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println();
    }

    if (firebaseData.streamTimeout())
    {
        Serial.println();
        Serial.println("Stream timeout, resume streaming...");
        Serial.println();
    }

    if (firebaseData.streamAvailable())
    {
        if (firebaseData.dataType() == "boolean")
        {
            if (firebaseData.boolData())
                Serial.println("Set " + nodeID + " to High");
            else
                Serial.println("Set " + nodeID + " to Low");
            digitalWrite(relePin, firebaseData.boolData());
        }
    }

}
