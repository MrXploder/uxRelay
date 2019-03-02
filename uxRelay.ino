#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

ESP8266WebServer server(80);
ESP8266WiFiMulti wifiMulti;

const char *mdnsName = "uxrelay1";

bool state = false;

// constants won't change. Used here to set a pin number:
const int ledPin =  LED_BUILTIN;// the number of the LED pin

// Variables will change:
int ledState = LOW;             // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 1000;           // interval at which to blink (milliseconds)

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("\r\n");

  pinMode(ledPin, OUTPUT);
  pinMode(D1, OUTPUT);

  startWiFi();
  startMDNS();
  startServer();
}

void loop() {
  unsigned long currentMillis = millis();

  server.handleClient();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
}

void startWiFi() {
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("Jumpitt Labs", "Jumpitt2015");
  wifiMulti.addAP("VTR-7236151", "hw9wcXhtHsm4");

  Serial.print("Connecting Wifi...");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void startMDNS() {
  MDNS.begin(mdnsName);
  Serial.print("mDNS responder started: http://");
  Serial.print(mdnsName);
  Serial.println(".local");
}

void startServer() {
  server.onNotFound([](){
    server.send(404, "text/plain", "404: File Not Found");
  });

  server.on("/toggle", HTTP_GET, [](){
    digitalWrite(D1, (state) ? HIGH : LOW);
    state = !state;
    server.sendHeader("Connection", "keep-alive");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "{'action':'toggled'}");
  });

  server.begin();
  Serial.println("HTTP server started.");
}