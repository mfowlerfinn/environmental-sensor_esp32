/*
  Initial release:  06/09/2018
  Last update:      06/09/2018
  Author: Matt FowlerFinn
  Platforms: ESP32 oled board by heltec automation
  ------------------------------------------------------------------------
  Description: 
  portable enviromental sensor to display temperature and humidity on integrated display

  Sensor:
  BME280 with i2c bus
  ------------------------------------------------------------------------
  License:
  Released under the MIT license. Please check LICENSE.txt for more
  information.  All text above must be included in any redistribution. 
*/

//environmental sensor
//by Matt FowlerFinn
//
//last updated
//hardware:
//esp32 oled board by "heltec automation"
//bme280 with i2c bus

#include <EnvironmentCalculations.h>  //"BME280" library by "Tyler Glen" v2.3.0
#include <BME280I2C.h>                //"BME280" library by "Tyler Glen" v2.3.0
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>             //"ADAFRUIT GFX Library" by "Adafruit" v1.2.3
#include <Adafruit_SSD1306.h>         //"ADAFRUIT SSD1306" library by "Adafruit" v1.1.2
#include <Fonts/FreeSans12pt7b.h>     //"ADAFRUIT GFX Library" by "Adafruit" v1.2.3
#include <Fonts/FreeSansBold9pt7b.h>  //"ADAFRUIT GFX Library" by "Adafruit" v1.2.3

#define OLED_RESET 16

#define SERIAL_BAUD 115200
BME280I2C bme;    // Default : forced mode, standby time = 1000 ms
                  // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
float tempc, humd, pre;

void setup()   {     
  Serial.begin(SERIAL_BAUD);

  while(!Serial) {} // Wait
             
  Wire.begin(4,15);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3c

    while(!bme.begin())
  {
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }

  // bme.chipID(); // Deprecated. See chipModel().
  switch(bme.chipModel())
  {
     case BME280::ChipModel_BME280:
       Serial.println("Found BME280 sensor! Success.");
       break;
     case BME280::ChipModel_BMP280:
       Serial.println("Found BMP280 sensor! No Humidity available.");
       break;
     default:
       Serial.println("Found UNKNOWN sensor! Error!");
  }
  
  display.setTextColor(WHITE);
  display.clearDisplay();
  
  //version annotation
  display.setCursor(0,0);
  display.println("environmental sensor");
  display.setCursor(0,15);
  display.println("v2.0");
  display.setCursor(0,50);
  display.println("Fowler-Finn Lab");
  display.display();
  display.setFont(&FreeSansBold9pt7b);
  display.setTextSize(1);
  delay(2000);
  
}

//////////////////////////////////////////////////////////////////
void printBME280Data
(
   Stream* client
)
{
   float temp(NAN), hum(NAN), pres(NAN);

   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);

   bme.read(pres, temp, hum, tempUnit, presUnit);
   tempc = temp;
   humd = hum;
   pre = pres;
   pre = pre/1000;
   client->print("Temp: ");
   client->print(temp);
   client->print("°"+ String(tempUnit == BME280::TempUnit_Celsius ? "C" :"F"));
   client->print("\t\tHumidity: ");
   client->print(hum);
   client->print("% RH");
   client->print("\t\tPressure: ");
   client->print(pres);
   client->print(" Pa");

   EnvironmentCalculations::AltitudeUnit envAltUnit  =  EnvironmentCalculations::AltitudeUnit_Meters;
   EnvironmentCalculations::TempUnit     envTempUnit =  EnvironmentCalculations::TempUnit_Celsius;

   float altitude = EnvironmentCalculations::Altitude(pres, envAltUnit);
   float dewPoint = EnvironmentCalculations::DewPoint(temp, hum, envTempUnit);
   float seaLevel = EnvironmentCalculations::EquivalentSeaLevelPressure(altitude, temp, pres);
   // seaLevel = EnvironmentCalculations::SealevelAlitude(altitude, temp, pres); // Deprecated. See EquivalentSeaLevelPressure().

   client->print("\t\tAltitude: ");
   client->print(altitude);
   client->print((envAltUnit == EnvironmentCalculations::AltitudeUnit_Meters ? "m" : "ft"));
   client->print("\t\tDew point: ");
   client->print(dewPoint);
   client->print("°"+ String(envTempUnit == EnvironmentCalculations::TempUnit_Celsius ? "C" :"F"));
   client->print("\t\tEquivalent Sea Level Pressure: ");
   client->print(seaLevel);
   client->println(" Pa");

   delay(1000);
}
  
void refresh(){
  display.setFont(&FreeSans12pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.setCursor(0,18);
  display.print(tempc,1);
  display.println("c");

  display.setCursor(0,42);
  display.print(humd,1);
  display.println("% RH");
  
  display.setFont();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  display.setCursor(0,50);
  display.print(pre,1);
  display.print(" KPa");

  display.setCursor(90,0);
  display.println("FF lab");
  display.setCursor(90,10);
  display.println("rocks!");
  
  display.display();
}

void loop(){
    printBME280Data(&Serial);
    refresh(); 
    }

