#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Replace with your network credentials
const char* ssid = "";
const char* password = "";
const char* mdnsName = "nodemcu";

int led = 0;
String webPage = "";

MDNSResponder mdns;
ESP8266WebServer server(80);

void setup() {
  webPage += "<h1>Arduino core for ESP8266 WiFi chip</h1><p><a href=\"on\"><button>On</button></a><a href=\"off\"><button>Off</button></a></p>";  
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  WiFi.begin(ssid, password);
  Serial.begin(115200); 
  Serial.println("Setting up");
  delay(5000);
  // Wait for connection and flash led till it connects
  // when connected; turn the led off
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led, HIGH);
    delay(200);
    digitalWrite(led, LOW);
    delay(200);
    Serial.print(".");
  }
  
  Serial.print("Connected to ");  Serial.println(ssid);
  Serial.print("IP address: ");   Serial.println(WiFi.localIP());

  // lets mDNS nodemcu on the received ip
  if (mdns.begin(mdnsName, WiFi.localIP())) {
    Serial.println("mDNS started for " + mdnsName + ".local");
    MDNS.addService("http", "tcp", 80);
    Serial.println("DNS-SD started for http / tcp / 80");
  }
  
  server.on("/", []() {
    server.send(200, "text/html", webPage);
  });
  server.on("/on", []() {
    server.send(200, "text/html", webPage);
    digitalWrite(led, HIGH);
  });
  server.on("/off", [](){
    server.send(200, "text/html", webPage);
    digitalWrite(led, LOW);
  });
  
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop() {
  server.handleClient();
} 
