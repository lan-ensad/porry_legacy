//https://wiki.dfrobot.com/FireBeetle_ESP32_IOT_Microcontroller(V3.0)__Supports_Wi-Fi_&_Bluetooth__SKU__DFR0478

#include <Arduino.h>
#include <analogWrite.h>
#include <WiFi.h>
#include <PubSubClient.h>

//----------------------------------
//        MQTT CONTROLLER
const char* ssid = "WIFI-SSID";
const char* password =  "WIFI-PASSWORD";
const char* mqttServer = "RASPBERRY-IP";
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
//     WIFI CONNECT / RECONNECT
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
  client.subscribe("fans/01");
  client.subscribe("fans/02");
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESPfans")) {
      Serial.println("connected");
      client.subscribe("fans/01");
      client.subscribe("fans/02");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
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
    if (fan01) {
      digitalWrite(FAN01, HIGH);
    } else {
      digitalWrite(FAN01, LOW);
    }
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
    if (fan01) {
      digitalWrite(FAN02, HIGH);
    } else {
      digitalWrite(FAN02, LOW);
    }
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
  pinMode(FAN01, OUTPUT); pinMode(FAN02, OUTPUT); pinMode(FAN03, OUTPUT); pinMode(FAN04, OUTPUT); pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(FAN01, LOW); digitalWrite(FAN02, LOW); digitalWrite(FAN03, LOW); digitalWrite(FAN04, LOW);

  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (fan01) {
    digitalWrite(FAN01, HIGH);
  } else {
    digitalWrite(FAN01, LOW);
  }
  if (fan02) {
    digitalWrite(FAN02, HIGH);
  } else {
    digitalWrite(FAN02, LOW);
  }
}
