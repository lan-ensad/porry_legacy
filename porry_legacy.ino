//https://wiki.dfrobot.com/FireBeetle_ESP32_IOT_Microcontroller(V3.0)__Supports_Wi-Fi_&_Bluetooth__SKU__DFR0478

#include <Arduino.h>
#include <analogWrite.h>
#include <WiFi.h>
#include <PubSubClient.h>

//----------------------------------
//        MQTT CONTROLLER
const char* ssid = "SSID";
const char* password =  "PASSWORD";
const char* mqttServer = "RASPBERRY_IP";
const int mqttPort = 1883;
String msg;
//----------------------------------
//        OUTPUT PIN
const int FAN01 = 26;
const int FAN02 = 25;
const int FAN03 = 27;
const int FAN04 = 9;
bool fan01, fan02 = false;

WiFiClient espClient;
PubSubClient client(espClient);

//----------------------------------
//         WIFI RECONNECT
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Connected to AP successfully!");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin(ssid, password);
}
//----------------------------------
//            CALLBACK
void callback(char* topic, byte* payload, unsigned int length) {
  String top = topic;
  Serial.print("Message arrived in topic: ");
  Serial.println(top);
  Serial.print("Message: ");
  //==============
  //=== FAN 01 ===
  if (top == "fans/01") {
    fan01 = !fan01;
    if(fan01){digitalWrite(FAN01, HIGH);}else{digitalWrite(FAN01, LOW);}
    msg = "";
    for (int i = 0; i < length; i++) {
      msg += (char)payload[i];
    }
    Serial.print(msg); Serial.print("\t \t"); Serial.print("Fan 01 : "); Serial.println(fan01);
  }
  //==============
  //=== FAN 02 ===
  else if (top == "fans/02") {
    fan02 = !fan02;
    if(fan01){digitalWrite(FAN02, HIGH);}else{digitalWrite(FAN02, LOW);}
    msg = "";
    for (int i = 0; i < length; i++) {
      msg += (char)payload[i];
    }
    Serial.print(msg); Serial.print("\t \t"); Serial.print("Fan 02 : "); Serial.println(fan02);
  }
  //==============
  Serial.println();
  Serial.println("-----------------------");
}

void setup() {
  pinMode(FAN01, OUTPUT);pinMode(FAN02, OUTPUT);pinMode(FAN03, OUTPUT);pinMode(FAN04, OUTPUT);pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(FAN01, LOW);digitalWrite(FAN02, LOW);digitalWrite(FAN03, LOW);digitalWrite(FAN04, LOW);

  Serial.begin(115200);
  
  WiFi.disconnect(true);
  delay(1000);
  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  client.subscribe("fans/01");
  client.subscribe("fans/02");
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
}
void loop() {
  client.loop();
  if(fan01){digitalWrite(FAN01, HIGH);}else{digitalWrite(FAN01, LOW);}
  if(fan02){digitalWrite(FAN02, HIGH);}else{digitalWrite(FAN02, LOW);}
}
