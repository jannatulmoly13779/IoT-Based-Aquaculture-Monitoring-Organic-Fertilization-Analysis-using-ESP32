#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "ThingSpeak.h" // Include the ThingSpeak library

// Pin Definitions
#define PH_SENSOR_PIN 33
#define TURBIDITY_SENSOR_PIN 34
#define WATER_LEVEL_SENSOR_PIN 32

LiquidCrystal_I2C lcd(0x27, 20, 4); // Ensure library is ESP32-compatible

namespace pin {
    const byte tds_sensor = 35;
}

namespace device {
    float aref = 3.3; // Vref, this is for 3.3v compatible controller boards.
}

namespace sensor {
    int ec = 0;
    unsigned int tds = 0;
    float ecCalibration = 1;
}

float calibration_value = 101.34 + 1.7;

// WiFi and ThingSpeak credentials
const char* ssid = "Sabbir";           // Replace with your WiFi SSID
const char* password = "19122918";   // Replace with your WiFi Password
unsigned long channelID = 2535462;   // Replace with your ThingSpeak Channel ID
const char* writeAPIKey = "DYLM3BJRLY18MAMF";    // Replace with your ThingSpeak Write API Key

WiFiClient client;

void setup() {
  pinMode(PH_SENSOR_PIN, INPUT);
  pinMode(TURBIDITY_SENSOR_PIN, INPUT);
  pinMode(WATER_LEVEL_SENSOR_PIN, INPUT);

  lcd.init();
  lcd.backlight();
  Serial.begin(115200); // Higher baud rate for ESP32
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to WiFi");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");

  ThingSpeak.begin(client); // Initialize ThingSpeak
  delay(2000);
  lcd.clear();
}

void loop() {
  // Read pH Value
  float phValue = readPH(PH_SENSOR_PIN);
  lcd.setCursor(0, 0);
  lcd.print("pH Value: ");
  lcd.print(phValue, 2); // Show 2 decimal places

  // Read TDS Value
  readTdsQuick();

  // Read Turbidity
  int ntu = readTurbidity(TURBIDITY_SENSOR_PIN);
  lcd.setCursor(0, 2);
  lcd.print("Turbidity: ");
  lcd.print(ntu);
  lcd.setCursor(11, 2);
  if (ntu < 10)
    lcd.print("VeryClean ");
  else if (ntu < 30)
    lcd.print("NormClean ");
  else
    lcd.print("VeryDirty");

  // Read Water Level
  int waterLevel = analogRead(WATER_LEVEL_SENSOR_PIN);
  lcd.setCursor(0, 3);
  lcd.print("Water Level: ");
  if (waterLevel == 0) {
    lcd.print("Empty  ");
  } else if (waterLevel < 350) {
    lcd.print("LOW    ");
  } else if (waterLevel < 510) {
    lcd.print("Medium ");
  } else {
    lcd.print("HIGH   ");
  }

  // Send data to ThingSpeak
  ThingSpeak.setField(1, phValue);         // Set Field 1 for pH
  ThingSpeak.setField(2, (long)sensor::tds);     // Set Field 2 for TDS
  ThingSpeak.setField(3, ntu);             // Set Field 3 for Turbidity
  ThingSpeak.setField(4, waterLevel);      // Set Field 4 for Water Level

  int response = ThingSpeak.writeFields(channelID, writeAPIKey);
  if (response == 200) {
    Serial.println("Data sent to ThingSpeak successfully!");
  } else {
    Serial.println("Error sending data to ThingSpeak. HTTP Response: " + String(response));
  }

  delay(15000); // Delay to avoid exceeding ThingSpeak update rate (15s minimum)
}

float readPH(int pin) {
  int buf[10];
  unsigned long avgValue = 0;

  // Collect 10 samples
  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(pin);
    delay(10);
  }

  // Sort and average middle values
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        int temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  for (int i = 2; i < 8; i++) {
    avgValue += buf[i];
  }

  float voltage = avgValue * 3.3 / 4096 / 6; // Convert for ESP32 ADC
  return -5.70 * voltage + calibration_value;
}

int readTurbidity(int pin) {
  int adcValue = analogRead(pin);
  adcValue = min(adcValue, 208); // Clamp to max range
  return map(adcValue, 0, 208, 300, 0);
}

void readTdsQuick() {
  // Read the raw analog value and convert to voltage
  float rawEc = analogRead(pin::tds_sensor) * device::aref / 4096.0;

  // Debugging: Print the raw analog value
  Serial.print(F("Raw Analog Value: "));
  Serial.println(rawEc);

  // Adjust this offset based on the sensor's dry reading (without immersion)
  float offset = 0.14; // Set this to the observed raw analog value in air

  // Apply calibration and offset compensation
  sensor::ec = (rawEc * sensor::ecCalibration) - offset;

  // If the EC is below zero after adjustment, set it to zero
  if (sensor::ec < 0) sensor::ec = 0;

  // Convert voltage value to TDS value using a cubic equation
  sensor::tds = (133.42 * pow(sensor::ec, 3) - 255.86 * sensor::ec * sensor::ec + 857.39 * sensor::ec) * 0.5;

  // Debugging: Print the TDS and EC values
  Serial.print(F("TDS: "));
  Serial.println(sensor::tds);
  Serial.print(F("EC: "));
  Serial.println(sensor::ec, 2);

  // Update the LCD with TDS and EC values
  lcd.setCursor(0, 1);
  lcd.print("TDS: ");
  lcd.setCursor(4, 1);
  lcd.print(sensor::tds);
}
