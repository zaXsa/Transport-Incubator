/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

/*  
 *  How I2C Communication Protocol Works - Example01
 *  
 *   by Dejan Nedelkovski, www.HowToMechatronics.com 
 *   
 */
#include <Wire.h>
//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio


int ADXLAddress = 0x27; // Device address in which is also included the 8th bit for selecting the mode, read in this case.
int X0,X1,X_out;
void setup() {
  Wire.begin(); // Initiate the Wire library
  Serial.begin(9600);
  delay(100);
  
  // Enable measurement
}
void loop() {
  uint8_t buf[4];
  Wire.beginTransmission(ADXLAddress);
  Wire.write(ADXLAddress);   // MSB
  //Wire.endTransmission();
  Wire.requestFrom(ADXLAddress,4 );
  int i = 0;
  while (i<4) {
    if (Wire.available()){
      buf[i]=Wire.read();
      i+=1;
      delay(5);
    }
  }
Wire.endTransmission();
Serial.print(buf[0]);
Serial.print("\t");
Serial.print(buf[1]);
Serial.print("\t");
Serial.print(buf[2]);
Serial.print("\t");
Serial.print(buf[3]);
Serial.print("\n");
  int reading_hum = (buf[0] << 8) + buf[1];
  double humidity = reading_hum / 16382.0 * 100.0;
  Serial.print("humidity: ");
  Serial.print(humidity);
  Serial.print("\n");

  int reading_temp = (buf[2] << 6) + (buf[3] >> 2);
  double temperature = reading_temp / 16382.0 * 165.0 - 40;
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("\n");  

  delay(1000);
}
