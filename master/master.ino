#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
// LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27 or 0x3F
// nRF24L01 Setup
RF24 radio(7, 8);             // CE=7, CSN=8 pins
const byte address[6] = "00001";   // Address to listen to
// Voltage Sensor Setup
const int vs = A0;
float vo;
float vsv;
const float factor = 4.992;
const float vcc = 5.07;
// Sensor Variables
int c;
long current;
float power, volt;
// Energy Variables
float localEnergy = 0.0;          // Local energy in watt-hours
unsigned long previousMillis = 0;
const float timeInterval = 1.0 / 3600.0; // Time interval in hours (1 second)
// Structure to receive sensor data
struct SensorData {
  float voltage;
  float current;
  float power;
  float energy;
};
SensorData remoteData;  // Data from transmitter
SensorData localData;   // Data collected locally
// Threshold for energy difference to flag theft
const float energyThreshold = 1.0;  // 1 Wh tolerance for mismatch

const int sensorPin = A1; // Analog pin connected to sensor
const float sensitivity = 0.185; // Sensitivity for 5A version in V/A
const int numSamples = 100; // Number of samples for averaging

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
void loop() {
  // Receive data from the transmitter
  if (radio.available()) {
    radio.read(&remoteData, sizeof(remoteData));
    // Print received data
    Serial.println("\n--- Remote Data Received ---");
    printData("Remote", remoteData);
  }
  // Read local sensor data
  readLocalSensorData();
  // Compare energy values for theft detection
  compareEnergy(remoteData.energy, localEnergy);
  // Display local and remote energy data on LCD
  displayDataOnLCD();
}
// Function to read local sensor data
void readLocalSensorData() {
  // current = 0;
  // for (int i = 0; i < 200; i++) {
  //   c = analogRead(A1); // Read current sensor
  //   current += c;
  //   delay(1);
  // }
  // // Convert to actual current value
  // current = ((current / 200.0) - 534);
  // if (current < 1) current = 0;
  // current *= 7;
  float voltage1 = 0;
  float current = 0;

  // Take multiple samples and average them
  for (int i = 0; i < numSamples; i++) {
    voltage1 += analogRead(sensorPin) * (5.0 / 1023.0); // Convert analog reading to voltage
    delay(1);
  }
  voltage1 /= numSamples;

  // ACS712 outputs 2.5V at 0A, subtract this offset
  float offsetVoltage = 2.5;
  float voltageDifference = voltage1 - offsetVoltage;

  // Calculate current
  current = -((voltageDifference / sensitivity)-0.150);


  // Read voltage sensor
  vsv = analogRead(vs);
  vo = (vsv / 1024.0) * vcc;
  volt = vo * factor;
  // Populate structure with local data
  localData.voltage = volt;
  localData.current = current;
  localData.power = (localData.voltage * localData.current);
  // Calculate local energy consumption over time
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    localEnergy += (localData.power * timeInterval);
    localData.energy = localEnergy;
    previousMillis = currentMillis;
  }
  // Print local data to Serial Monitor
  Serial.println("\n--- Local Data Collected ---");
  printData("Local", localData);
}
// Function to compare energy data and detect theft
void compareEnergy(float remoteEnergy, float localEnergy) {
  float energyDiff = abs(remoteEnergy - localEnergy);

  if (energyDiff > energyThreshold) {
    Serial.print("Energy Mismatch Detected: ");
    Serial.print(energyDiff, 2);
    Serial.println(" Wh");
    lcd.setCursor(0, 1);
    lcd.print("Theft Suspected! ");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Energy: OK           ");
  }
}
// Function to display local and remote data on LCD
void displayDataOnLCD() {
  lcd.setCursor(0, 0);
  lcd.print("E: ");
  lcd.print(localEnergy, 2);
  lcd.print("/RX:");
  lcd.print(remoteData.energy, 2);
  lcd.print("Wh ");
}
// Function to print sensor data to Serial Monitor
void printData(String source, SensorData data) {
  Serial.print(source);
  Serial.print(" -> Voltage: ");
  Serial.print(data.voltage, 2);
  Serial.print(" V, Current: ");
  Serial.print(data.current, 2);
  Serial.print(" A, Power: ");
  Serial.print(data.power, 2);
  Serial.print(" W, Energy: ");
  Serial.print(data.energy, 2);
  Serial.println(" Wh");
}