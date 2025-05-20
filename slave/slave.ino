#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
// LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27 or 0x3F
// nRF24L01 Setup
RF24 radio(7, 8);                  // CE=7, CSN=8 pins
const byte address[6] = "00001";   // Address for communication
// Voltage Sensor Setup
const int vs = A0;                 // Voltage sensor on A0
float vo;
float vsv;
const float factor = 4.992;        // vi/vo = factor
const float vcc = 4.97;            // Arduino VCC for sensor calculation
// Sensor Variables
int c;
long current;
float power, volt;
// Energy Variables
float energyConsumed = 0.0;          // Energy in watt-hours
unsigned long previousMillis = 0;
const float timeInterval = 1.0 / 3600.0; // Time interval in hours (1 second)
struct SensorData {
  float voltage;
  float current;
  float power;
  float energy;
};
SensorData data; // Data structure to transmit

const int sensorPin = A2; // Analog pin connected to sensor
const float sensitivity = 0.185; // Sensitivity for 5A version in V/A
const int numSamples = 100; // Number of samples for averaging

void setup() {
  lcd.init();          // Initialize the I2C LCD
  lcd.backlight();     // Turn on the LCD backlight
  Serial.begin(9600);  // Start serial communication
  radio.begin();       // Initialize nRF24L01
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening(); 
}
void loop() {
  // Read sensor data and update structure
  readSensorData();
  // Send data to receiver
  bool success = radio.write(&data, sizeof(data));
  // Display data on LCD
  displayDataOnLCD();
  // Print data to Serial Monitor
  printDataToSerial();
  delay(100);
}
// Function to read and calculate sensor data
void readSensorData() {
  // Reset current
  // current = 0;
  // // Take 200 samples for accuracy
  // for (int i = 0; i < 200; i++) {
  //   c = analogRead(A2); // Read current sensor from A1
  //   current += c;
  //   delay(1);
  // }
  // // Convert to actual current value
  // current = ((current / 200.0) - 534);
  // if (current < 1) current = 0;
  // current *= 7; // Calibration factor
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
  current = ((voltageDifference / sensitivity)+0.6);
  if (current>1.5){
    current -=1.5;
  }
  // Read voltage sensor
  vsv = analogRead(vs);
  vo = (vsv / 1023.0) * vcc;
  volt = vo * factor;
  // Populate data structure
  data.voltage = volt;
  data.current = current;
  data.power = (data.voltage * data.current);
  // Calculate and update energy consumption over time
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) { // 1 second interval
    energyConsumed += (data.power * timeInterval); // P × t
    data.energy = energyConsumed;
    previousMillis = currentMillis;
  }
}
// Function to display data on LCD
void displayDataOnLCD() {
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(data.voltage, 2);
  lcd.print("V ");
  lcd.setCursor(8, 0);
  lcd.print("A:");
  lcd.print(data.current, 2);
  lcd.print("A ");
  lcd.setCursor(0, 1);
  lcd.print("E:");
  lcd.print(data.energy, 2);
  lcd.print("Wh ");
}
// Function to print data to Serial Monitor
void printDataToSerial() {
  Serial.print("Voltage: ");
  Serial.print(data.voltage, 2);
  Serial.print(" V, Current: ");
  Serial.print(data.current, 2);
  Serial.print(" A, Power: ");
  Serial.print(data.power, 2);
  Serial.print(" W, Energy: ");
  Serial.print(data.energy, 2);
  Serial.println(" Wh");
}