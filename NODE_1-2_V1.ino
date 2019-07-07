#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <stdlib.h>

//RGB 1-2//
 #define S0 16
 #define S1 5
 #define S2 4
 #define S3 0
 #define S1OUT 2
 #define S2OUT 14
 
 int frequencyR1,frequencyG1,frequencyB1,frequencyC1 = 0;
 int frequencyR2,frequencyG2,frequencyB2,frequencyC2 = 0;
 String color1,color2;
 
const char ssid[] = "Gamadita.net"; /////CHANGE THIS////
const char pass[] = "bandung123"; /////CHANGE THIS////
const char* deviceName = "node1"; /////CHANGE THIS////

//Static IP address configuration
IPAddress staticIP(192, 168, 0, 21); /////CHANGE THIS (STATIC IP)////
IPAddress gateway(192, 168, 0, 1);   /////CHANGE THIS////
IPAddress subnet(255, 255, 255, 0); 
IPAddress dns(8, 8, 8, 8);  //DNS

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("checking wifi...");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("node1", "try", "try")) {            /////CHANGE THIS////
    Serial.print("\nconnecting...");
    delay(1000);
  }
  Serial.println("\nconnected!");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

void setup() {
  //RGB //
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(S1OUT, INPUT);
  pinMode(S2OUT, INPUT);
      
  // Setting frequency-scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,HIGH);
  
  Serial.begin(115200);
  WiFi.disconnect();
  WiFi.hostname(deviceName);
  //WiFi.config(staticIP, dns, gateway); //////OFF THIS////
  WiFi.begin(ssid, pass);
  
  client.begin("192.168.0.5", net); /////CHANGE THIS////
  connect();
}

void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }
  
//////////////////////////////////////////////////RGB CODE//////////////////////////////////////////
  // Setting red filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  frequencyR1 = pulseIn(S1OUT, LOW);
  frequencyR2 = pulseIn(S2OUT, LOW);
  // Printing the value on the serial monitor  
  

  // Setting Green filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequencyG1 = pulseIn(S1OUT, LOW);
  frequencyG2 = pulseIn(S2OUT, LOW);
  // Printing the value on the serial monitor
      
  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequencyB1 = pulseIn(S1OUT, LOW);
  frequencyB2 = pulseIn(S2OUT, LOW);
  // Printing the value on the serial monitor
  
/////////////////////////////////////LOGIC/////////////////////////////////////////////////////////////
 color1 = "no detection";
 color2 = "no detection";
 if (frequencyR1 >= 1000 && frequencyG1 >=1000 && frequencyB1 >=1000) {
  color1 = "Black";
 } else if (frequencyR1 <= 500 && frequencyG1 >=1000 && frequencyB1 >=1000) {
  color1 = "Red";
 }

  if (frequencyR2 >= 1000 && frequencyG2 >=1000 && frequencyB2 >=1000) {
  color2 = "Black";
 } else if (frequencyR2 <= 500 && frequencyG2 >=1000 && frequencyB2 >=1000) {
  color2 = "Red";
 }
///////////////////////////////////////////Publishing////////////////////////////////////////////////////////

  // publish a message roughly every second.
  if (millis() - lastMillis > 500) {
    lastMillis = millis();
    char buffer[20];
    Serial.println("R1 = " + frequencyR1);
    Serial.println("G1 = " + frequencyG1);
    Serial.println("B1 = " + frequencyB1);
    Serial.println("R2 = " + frequencyR2);
    Serial.println("G2 = " + frequencyG2);
    Serial.println("B2 = " + frequencyB2);
    client.publish("Node1/Color1", color1);
    client.publish("Node1/Color2", color2);
    //client.publish("Node1/R", itoa(frequencyR,buffer,10));
    //client.publish("Node1/G", itoa(frequencyG,buffer,10));
    //client.publish("Node1/B", itoa(frequencyB,buffer,10));
    Serial.println("publishing...");
  }
}
