#include <Servo.h>

#define LDR_PIN 1

#define HEAT_PIN 5
#define FAN_PIN 6
#define LIGHT_PIN 7
#define WATER_MOTOR_BUFFER 4

#define LIGHT_STATE 22
#define COOLER_STATE 24
#define HEATER_STATE 26
#define WATER_MOTOR_STATE 28

#define MAX_ADC_READING 1023
#define ADC_REF_VOLTAGE 5
#define LUX_CALC_SCALAR 12518931
#define LUX_CALC_EXPONENT -1.405
#define REF_RESISTANCE 5000

#include <SPI.h>
#include "cactus_io_BME280_SPI.h"

#define BME_SCK 13// Serial Clock 
#define BME_MISO 12// Serial Data Out 
#define BME_MOSI 11// Serial Data In 
#define BME_CS 10// Chip Select 

// Create BME280 object 
// BME280_SPI bme(BME_CS); // Using Hardware SPI 
BME280_SPI bme(BME_CS,BME_MOSI,BME_MISO,BME_SCK);

const int sensorPin= 0;
int liquid_level;

Servo myservo;

int pos = 0;

void setup() {
  Serial.begin(9600);
  myservo.attach(9);
  pinMode(sensorPin, INPUT);
  pinMode(HEAT_PIN, OUTPUT); //For Heat
  pinMode(FAN_PIN, OUTPUT); //For Fan

  pinMode(LIGHT_STATE, INPUT);
  pinMode(COOLER_STATE, INPUT);
  pinMode(HEATER_STATE, INPUT);
  pinMode(WATER_MOTOR_STATE, INPUT);

  pinMode(LIGHT_PIN, OUTPUT);

  
  if (!bme.begin()) { 
    Serial.println("Could not find a valid BME280 sensor, check wiring!"); 
    while (1); 
  } 

  bme.setTempCal(-1);// Sensor was reading high so offset by 1 degree C 
}

void loop() {
    Serial.println(getTemperature()+","+getLight()+","+getAtmosphericHumidity()+","+getCoolerState()+","+getHeaterState()+","+getWaterMotorState()+","+getLightState());
    
    if (Serial.available() > 0) {
      String controlCommand = Serial.readString();
      handleControl(controlCommand.toInt());
    }
    
    delay(1000);
}

void handleControl(int command){
  switch(command){
    case 100:
      onWater();
      break;
    case 101:
      offWater();
      break;
    case 200:
      coolFarm();
      break;
    case 201:
      offHeaterCooler();
      break;
    case 300:
      onLight();
      break;
    case 301:
      offLight();
      break;
    case 400:
      heatFarm();
      break;
    case 401:
      offHeaterCooler();
      break;
    default:
      break;
  }
}

String getLight(){
  int ldrRawData = analogRead(LDR_PIN);
  float resistorVoltage = (float)ldrRawData / MAX_ADC_READING * ADC_REF_VOLTAGE;
  float ldrVoltage = ADC_REF_VOLTAGE - resistorVoltage;
  float ldrResistance = ldrVoltage/resistorVoltage * REF_RESISTANCE;
  float ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);
  String ldrLuxStr = String(ldrLux); 
  return ldrLuxStr;
}

String getSoilHumidity(){
  liquid_level= analogRead(sensorPin);
  String soil = String(liquid_level);
  return soil;
}

String getTemperature(){
  bme.readSensor();
  float temperature = bme.getTemperature_C();
  String temp = String(temperature);
  return temp;
}

String getAtmosphericHumidity(){
  bme.readSensor();
  float atmospheric_humidity = bme.getHumidity();
  String atmospheric_str = String(atmospheric_humidity);
  return atmospheric_str;
}

void onWater(){
  for (pos = 0; pos <= 180; pos += 1) {
    myservo.write(pos);
    delay(15);
  }
}

void offWater(){
  for (pos = 180; pos >= 0; pos -= 1) {
    myservo.write(pos);  
    delay(15);
  }
}

void onLight(){
  digitalWrite(LIGHT_PIN, HIGH);
}

void offLight(){
  digitalWrite(LIGHT_PIN, LOW);
}

String getCoolerState(){
  boolean coolerState = digitalRead(LIGHT_STATE);
  String coolerStr = String(coolerState);
  return coolerStr;
}

String getHeaterState(){
  boolean heaterState = digitalRead(HEATER_STATE);
  String heaterStr = String(heaterState);
  return heaterStr;
}

String getWaterMotorState(){
  boolean waterState = digitalRead(WATER_MOTOR_STATE);
  String waterString = String(waterState);
  return waterString;
}

String getLightState(){
  boolean lightState = digitalRead(LIGHT_STATE);
  String lightStr = String(lightState);
  return lightStr;
}

void coolFarm(){
  digitalWrite(FAN_PIN, HIGH);
  digitalWrite(HEAT_PIN, LOW);
}

void heatFarm(){
  digitalWrite(HEAT_PIN, HIGH);
  digitalWrite(FAN_PIN, LOW);
}

void offHeaterCooler(){
  digitalWrite(HEAT_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);
}

