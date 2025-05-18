// Code written by Saksham Rajpal [ iOT India ] !!!
// Youtube video link : https://www.youtube.com/@iOT_India

#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID ""       // your blynk template ID
#define BLYNK_TEMPLATE_NAME ""     // your blynk template name
#define BLYNK_AUTH_TOKEN ""        // your blynk authentication token  

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

#define DHTPIN 23 // Pin connected to DHT11 sensor
#define DHTTYPE DHT11 // Define the sensor type (DHT11)

DHT dht(DHTPIN, DHTTYPE);

unsigned int receivedValue = 0;

// WIFI DETAILS
char ssid[] = "********";        // your Wi-Fi name
char pass[] = "********";        // your Wi-Fi password

BlynkTimer timer;

BLYNK_WRITE(V0) {
  int value = param.asInt();
  Serial.print("Received Data - ");
  Serial.println(value);
  digitalWrite(15, value);
}

BLYNK_WRITE(V1) {
  int value = param.asInt();
  Serial.print("Received Data - ");
  Serial.println(value);
  digitalWrite(2, value);
}

BLYNK_WRITE(V2) {
  int value = param.asInt();
  Serial.print("Received Data - ");
  Serial.println(value);
  digitalWrite(4, value);
}

BLYNK_WRITE(V3) {
  int value = param.asInt();
  Serial.print("Received Data - ");
  Serial.println(value);
  digitalWrite(5, value);
}

void sendDHTData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if the readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print and send data to Blynk
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Blynk.virtualWrite(V4, temperature);

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Blynk.virtualWrite(V5, humidity);
}
 
void setup() {
  // Debug console
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // Use UART2: RX=16, TX=17
  Serial.println("Waiting for data from VC02...");
  
  // RELAY PINS (OUTPUT)
  pinMode(15, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  // PHYSICAL SWITCH PINS (INPUT)
  pinMode(13, INPUT_PULLUP);  // Active LOW switch
  pinMode(12, INPUT_PULLUP);  // Active LOW switch
  pinMode(14, INPUT_PULLUP);  // Active LOW switch
  pinMode(27, INPUT_PULLUP);  // Active LOW switch

  // Initialize DHT sensor
  dht.begin();
  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Set up a timer to fetch and send DHT data every 10 seconds
  timer.setInterval(10000L, sendDHTData);
}

void loop() {
  Blynk.run();
  timer.run();

  if (Serial2.available()) {
    // Read the incoming bytes
    byte highByte = Serial2.read();
    byte lowByte = Serial2.read();

    // Combine the two bytes into a single 16-bit value
    receivedValue = (highByte << 8) | lowByte;

    // Print the received value in HEX format
    Serial.print("Received HEX value: 0x");
    Serial.println(receivedValue, HEX);
  }

  // Check the physical switch states and update the relays
  static bool lastSwitchState13 = HIGH;
  static bool lastSwitchState12 = HIGH;
  static bool lastSwitchState14 = HIGH;
  static bool lastSwitchState27 = HIGH;

  bool currentSwitchState13 = digitalRead(13) == LOW; // Active LOW switch
  bool currentSwitchState12 = digitalRead(12) == LOW;
  bool currentSwitchState14 = digitalRead(14) == LOW;
  bool currentSwitchState27 = digitalRead(27) == LOW;

  // Switch 13 control
  if (currentSwitchState13 != lastSwitchState13) {
    digitalWrite(15, currentSwitchState13);  // Update relay 1
    Blynk.virtualWrite(V0, currentSwitchState13);
    lastSwitchState13 = currentSwitchState13;
  }

  // Switch 12 control
  if (currentSwitchState12 != lastSwitchState12) {
    digitalWrite(2, currentSwitchState12);  // Update relay 2
    Blynk.virtualWrite(V1, currentSwitchState12);
    lastSwitchState12 = currentSwitchState12;
  }

  // Switch 14 control
  if (currentSwitchState14 != lastSwitchState14) {
    digitalWrite(4, currentSwitchState14);  // Update relay 3
    Blynk.virtualWrite(V2, currentSwitchState14);
    lastSwitchState14 = currentSwitchState14;
  }

  // Switch 27 control
  if (currentSwitchState27 != lastSwitchState27) {
    digitalWrite(5, currentSwitchState27);  // Update relay 4
    Blynk.virtualWrite(V3, currentSwitchState27);
    lastSwitchState27 = currentSwitchState27;
  }

  // Control relays based on received UART data
  if (receivedValue == 0xAA11) {
    digitalWrite(15, HIGH);  
    Blynk.virtualWrite(V0, 1);
  } else if(receivedValue == 0xAA00) {
    digitalWrite(15, LOW);  
    Blynk.virtualWrite(V0, 0);
  } else if(receivedValue == 0xBB11) {
    digitalWrite(2, HIGH);  
    Blynk.virtualWrite(V1, 1);
  } else if(receivedValue == 0xBB00) {
    digitalWrite(2, LOW);  
    Blynk.virtualWrite(V1, 0);
  } else if(receivedValue == 0xCC11) {
    digitalWrite(4, HIGH);  
    Blynk.virtualWrite(V2, 1);
  } else if(receivedValue == 0xCC00) {
    digitalWrite(4, LOW); 
    Blynk.virtualWrite(V2, 0);
  } else if(receivedValue == 0xDD11) {
    digitalWrite(5, HIGH);  
    Blynk.virtualWrite(V3, 1);
  } else if(receivedValue == 0xDD00) {
    digitalWrite(5, LOW); 
    Blynk.virtualWrite(V3, 0);
  } else if(receivedValue == 0xAD11) { // TURNING ON ALL SWITCHES
    digitalWrite(15, HIGH);
    digitalWrite(2, HIGH); 
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH); 
    Blynk.virtualWrite(V0, 1);
    Blynk.virtualWrite(V1, 1);
    Blynk.virtualWrite(V2, 1);
    Blynk.virtualWrite(V3, 1);
  } else if(receivedValue == 0xAD00) { // TURNING OFF ALL SWITCHES
    digitalWrite(15, LOW);
    digitalWrite(2, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW); 
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V2, 0);
    Blynk.virtualWrite(V3, 0);
  }

  // Reset received value
  delay(10);
  receivedValue = 0;
}
