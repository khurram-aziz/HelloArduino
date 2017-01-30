#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>

// Replace with your network credentials and mqtt settings
const char* ssid = "";
const char* password = "";
const char* mqttServer = "";
const char* mqttUser = "";
const char* mqttPassword = "";

int led = 0;

WiFiClient espClient;
PubSubClient client(espClient);
MDNSResponder mdns;

void setup(void) {
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
  
  Serial.print("Connected to "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());

  // lets mDNS nodemcu on the received ip
  if (mdns.begin("nodemcu", WiFi.localIP())) {
    Serial.println("mDNS started for nodemcu.local");
  }

  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}
 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived ["); Serial.print(topic); Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    Serial.println("Turning LED on");
    digitalWrite(led, LOW);   //ESP is active low
  }
  else if ((char)payload[0] == '0') {
    Serial.println("Turning LED off");
    digitalWrite(led, HIGH);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
      Serial.println("connected");
      client.publish("nodemcu", "Connected"); //publishing its status
      client.subscribe("test");               //subscribing to test
    }
    else {
      Serial.print("Failed, state="); Serial.print(client.state());
      Serial.println(" will try again in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected())
    reconnect();
  client.loop();
}

