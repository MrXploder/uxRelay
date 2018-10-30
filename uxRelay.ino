#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

ESP8266WebServer server(80);

const char *ssid = "Jumpitt Labs";
const char *password = "Jumpitt2015";
const char *mdnsName = "uxswitch1";

bool state = false;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("\r\n");

  pinMode(D1, OUTPUT);

  startWiFi();
  startMDNS();
  startServer();
}

void loop() {
  server.handleClient();
}

void startWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print('.');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
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
