#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "<SSID>"
#define STAPSK  "<PSK>"
#define TIMEOUT 60
#endif

const char* ssid = SSID;
const char* password = PSK;

ESP8266WebServer server(80);

const int led = 13;

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Trying to connect to WiFi [");
  
  int timeout_count = 0;
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED && timeout_count < TIMEOUT) {
    delay(500);
    Serial.print("#");
    timeout_count += 1;
  }

  if (timeout_count == TIMEOUT) {
    Serial.print("#");
    Serial.println("Failed to connect to WiFi please check your credentials.");
    return;
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  Serial.print("]");

  server.on("/", []() {
    digitalWrite(led, 1);
    server.send(200, "text/plain", "The test succeeded...");
    digitalWrite(led, 0);
  });

  server.on("/test", []() {
    server.send(200, "text/plain", "That page works as well.");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
