#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define MAXCLIENTS 2

// Add your network credentials
const char* ssid = "";
const char* password = "";
const char* mdnsName = "nodemcu";
int led = 0;

MDNSResponder mdns;
ESP8266WebServer httpServer(80);
WiFiServer tcpServer(3000);
WiFiClient clients[MAXCLIENTS];

String webPage = ""; //we are using lambdas later

void setupWifi() {
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

  // lets mDNS on the received ip
  if (mdns.begin(mdnsName, WiFi.localIP())) {
    Serial.print("mDNS started for "); Serial.print(mdnsName); Serial.println(".local");
    MDNS.addService("http", "tcp", 80);
    Serial.println("DNS-SD started for http / tcp / 80");
    MDNS.addService("led", "tcp", 3000);
    Serial.println("DNS-SD started for led / tcp / 3000");
  }
}

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH); //lets find out if its working
  WiFi.begin(ssid, password);
  Serial.begin(115200); 
  Serial.println("Setting up");
  
  setupWifi();

  webPage = "<h1>Arduino core for ESP8266 WiFi chip</h1><p><a href=\"on\"><button>On</button></a><a href=\"off\"><button>Off</button></a></p>";
  httpServer.on("/", []() {
    httpServer.send(200, "text/html", webPage);
  });
  httpServer.on("/on", []() {
    httpServer.send(200, "text/html", webPage);
    Serial.println("LED is turned on");
    digitalWrite(led, HIGH);
  });
  httpServer.on("/off", []() {
    httpServer.send(200, "text/html", webPage);
    digitalWrite(led, LOW);
    Serial.println("LED is turned off");
  });

  //Servers will continue to work across disconnection
  httpServer.begin();
  tcpServer.begin();
  Serial.println("TCP and HTTP servers are started");
}
 
void loop() {
  if (WiFi.status() != WL_CONNECTED)
    setupWifi();

  if (tcpServer.hasClient()) {
    for (int i = 0; i < MAXCLIENTS; i++) {
      if (!clients[i] || !clients[i].connected()) {
        if (clients[i])
          clients[i].stop();
        clients[i] = tcpServer.available();
        continue;
      }
    }
    //no free spot
    WiFiClient c = tcpServer.available();
    c.stop();
  }
  for (int i = 0; i < MAXCLIENTS; i++) {
    if (clients[i] && clients[i].connected()){
      if (clients[i].available()) {
        while (clients[i].available()) {
          char command = clients[i].read();
          if (command == 'H') {
            digitalWrite(led, HIGH);
            Serial.println("LED is turned on");
          }
          else if (command == 'L') {
            digitalWrite(led, LOW);
            Serial.println("LED is turned off");
          }
        }
      }
    }
  }
  httpServer.handleClient();
} 
