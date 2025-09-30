#include <DHT.h>

// ----- DHT11 Setup -----
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ----- MQ2 Sensor -----
#define MQ2_PIN A0

// ----- Ultrasonic Sensor -----
#define TRIG_PIN 3
#define ECHO_PIN 4

// ----- Bluetooth & Serial -----
#define BAUD_RATE 9600

// ----- Setup -----
void setup() {
  Serial.begin(BAUD_RATE);       // For Serial Monitor & HC-05 Bluetooth
  dht.begin();

  pinMode(MQ2_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("System Ready.");
  printAvailableCommands();       // Show commands at startup
}

// ----- Distance Calculation Function -----
long getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout 30ms
  if (duration == 0) return -1;  // No object detected
  long distance = duration * 0.034 / 2;           // Convert to cm
  return distance;
}

// ----- Print Available Commands -----
void printAvailableCommands() {
  Serial.println("Available Commands:");
  Serial.println("TEMP  - Temperature");
  Serial.println("HUM   - Humidity");
  Serial.println("GAS   - Gas Sensor");
  Serial.println("DIST  - Distance");
  Serial.println("ALL   - All Sensor Data");
  Serial.println("------------------------");
}

// ----- Main Loop -----
void loop() {
  // Check if data is available from Serial or Bluetooth
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();  // Remove newline and spaces

    // ----- Handle Commands -----
    if (cmd.equalsIgnoreCase("TEMP")) {
      float t = dht.readTemperature();
      if (isnan(t)) Serial.println("ERR: TEMP READ FAILED");
      else Serial.print("TEMP: "), Serial.println(t);
    }
    else if (cmd.equalsIgnoreCase("HUM")) {
      float h = dht.readHumidity();
      if (isnan(h)) Serial.println("ERR: HUM READ FAILED");
      else Serial.print("HUM: "), Serial.println(h);
    }
    else if (cmd.equalsIgnoreCase("GAS")) {
      int gasVal = analogRead(MQ2_PIN);
      Serial.print("GAS: "), Serial.println(gasVal);
    }
    else if (cmd.equalsIgnoreCase("DIST")) {
      long dist = getDistanceCM();
      if (dist == -1) Serial.println("DIST: Out of range");
      else Serial.print("DIST: "), Serial.print(dist), Serial.println(" cm");
    }
    else if (cmd.equalsIgnoreCase("ALL")) {
      float t = dht.readTemperature();
      float h = dht.readHumidity();
      int gasVal = analogRead(MQ2_PIN);
      long dist = getDistanceCM();

      Serial.print("TEMP: "); Serial.print(t);
      Serial.print(" | HUM: "); Serial.print(h);
      Serial.print(" | GAS: "); Serial.print(gasVal);
      Serial.print(" | DIST: ");
      if (dist == -1) Serial.println("Out of range");
      else Serial.print(dist), Serial.println(" cm");
    }
    else {
      Serial.println("ERR: UNKNOWN COMMAND");
    }

    // ----- Print Commands After Each Response -----
    Serial.println();
    printAvailableCommands();
  }
}
