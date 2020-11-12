/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include "secure.h"
// /************************* WiFi Access Point *********************************/
// #define _WIFI_SSID       "...your SSID..."
// #define _WIFI_PASSWORD       "...your password..."
// /************************* Adafruit.io Setup *********************************/
// #define _AIO_SERVER      "io.adafruit.com"
// #define _AIO_SERVERPORT  1883                   // use 8883 for SSL
// #define _AIO_USERNAME    "...your AIO username (see https://accounts.adafruit.com)..."
// #define _AIO_KEY         "...your AIO key..."

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
//WiFiClient client;
// or... use WiFiClientSecure for SSL
WiFiClientSecure _VAR_WiFiClientSecure;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client _VAR_MQTT(&_VAR_WiFiClientSecure, _AIO_SERVER, _AIO_SERVERPORT, _AIO_USERNAME, _AIO_KEY);

// io.adafruit.com SHA1 fingerprint
static const char *_CONST_Fingerprint PROGMEM = "59 3C 48 0A B1 8B 39 4E 0D 58 50 47 9A 13 55 60 CC A0 1D AF";

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish _VAR_Photocell = Adafruit_MQTT_Publish(&_VAR_MQTT, _AIO_USERNAME "/feeds/test");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe _VAR_OnOffButton = Adafruit_MQTT_Subscribe(&_VAR_MQTT, _AIO_USERNAME "/feeds/test");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration

// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup()
{
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(_WIFI_SSID);

  WiFi.begin(_WIFI_SSID, _WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  _VAR_MQTT.subscribe(&_VAR_OnOffButton);

  // check the fingerprint of io.adafruit.com's SSL cert
  // needed, if not error connecting to AIO Server
  _VAR_WiFiClientSecure.setFingerprint(_CONST_Fingerprint);
}

uint32_t x = 0;

void loop()
{
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *_VAR_MQTT_Subscription;
  while ((_VAR_MQTT_Subscription = _VAR_MQTT.readSubscription(5000)))
  {
    if (_VAR_MQTT_Subscription == &_VAR_OnOffButton)
    {
      Serial.print(F("Got: "));
      Serial.println((char *)_VAR_OnOffButton.lastread);
    }
  }

  // Now we can publish stuff!
  Serial.print(F("\nSending photocell val "));
  Serial.print(x);
  Serial.print("...");
  if (!_VAR_Photocell.publish(x++))
  {
    Serial.println(F("Failed"));
  }
  else
  {
    Serial.println(F("OK!"));
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! _VAR_MQTT.ping()) {
    _VAR_MQTT.disconnect();
  }
  */
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect()
{
  int8_t _VARL_ret;

  // Stop if already connected.
  if (_VAR_MQTT.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t _VARL_retries = 3;
  while ((_VARL_ret = _VAR_MQTT.connect()) != 0)
  { // connect will return 0 for connected
    Serial.println(_VAR_MQTT.connectErrorString(_VARL_ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    _VAR_MQTT.disconnect();
    delay(5000); // wait 5 seconds
    _VARL_retries--;
    if (_VARL_retries == 0)
    {
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}




