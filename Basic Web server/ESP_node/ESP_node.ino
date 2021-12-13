//Libraries
#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>

//Sensor Libraries
#include <Adafruit_BMP280.h>
 
#define SS 5
#define RST 14
#define DI0 2
 
//#define TX_P 17
//#define ENCRYPT 0x78

//Lora Band
#define BAND 433E6

//Initialisation
#define rain_sensor 34
#define moisturePin 35             // moisteure sensor pin
Adafruit_BMP280 bmp;               // I2C

//BH1750 lightMeter;

//Lora Device Id
String LoRaMessage = "";
char device_id[12] = "MyDevice123";
 
 
void setup(){
  Serial.begin(115200);
  pinMode (rain_sensor, INPUT);
  pinMode (moisturePin, INPUT);

  while (!Serial);
  Serial.println(F("LoRa Sender"));
  LoRa.setPins(SS, RST, DI0);
  
  //LoRa.setTxPower(TX_P);
  //LoRa.setSyncWord(ENCRYPT);
 
  if (!LoRa.begin(BAND))
  {
    Serial.println(F("Starting LoRa failed!"));
    while (1);
    
  }
  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}
 
void loop()
{
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0F;
  float altitude = bmp.readAltitude(1013.25);
  int moisturePercentage = ( 100 - ( (analogRead(moisturePin) / 1023.00) * 100 ) );
  int rainfall = map(analogRead(rain_sensor), 780, 0, 0, 100);
  if (rainfall >= 100)
  {
    rainfall = 100;
  }
  if (rainfall <= 0)
  {
    rainfall = 0;
  }
  
//  float humidity = bmp.readHumidity();
 
//  double dewPoint = dewPointFast(temperature, humidity);
// 
//  int rainfall = map(analogRead(rain_sensor), 780, 0, 0, 100);
//  if (rainfall >= 100)
//  {
//    rainfall = 100;
//  }
//  if (rainfall <= 0)
//  {
//    rainfall = 0;
//  }
 
//  float lux = lightMeter.readLightLevel();
 
  Serial.print(F("Device ID: "));
  Serial.println(device_id);
 
  Serial.print(F("Temperature = "));
  Serial.print(temperature);
  Serial.println(F("*C"));
 
  Serial.print(F("Pressure = "));
  Serial.print(pressure);
  Serial.println(F("hPa"));
 
  Serial.print(F("Approx. Altitude = "));
  Serial.print(altitude);
  Serial.println(F("m"));

  Serial.print("Soil Moisture is  = ");
  Serial.print(moisturePercentage);
  Serial.println("%");
  
  Serial.print(F("Rainfall = "));
  Serial.print(rainfall);
  Serial.println(F("%"));
 
 
  Serial.println();
 
  LoRaMessage = String(device_id) + "/" + String(temperature) + "&" + String(pressure)
                + "#" + String(altitude) + "@" + String(moisturePercentage) 
                + "$" + String(rainfall);
 
  // send packet
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();
  delay(10000);
}
 
 
//double dewPointFast(double celsius, double humidity)
//{
//  double a = 17.271;
//  double b = 237.7;
//  double temp = (a * celsius) / (b + celsius) + log(humidity * 0.01);
//  double Td = (b * temp) / (a - temp);
//  return Td;
//}
