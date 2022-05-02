// tmp006 with Arduino (Lux meter)
// by Hassan Salmani
// Created 2 Feb 2022

// https://energia.nu/guide/libraries/wire/
#include <Wire.h>
double DieData;
double S_0 = 6.4;
double A_1 = 0.00175;
double A_2 = -0.00001678;
double T_ref = 298.15;
double B_0 = -0.0000294;
double B_1 = -0.00000057;
double B_2 = 0.00000000463;
double C_2 = 13.4;
//double T_m;
void setup() 
{
  Serial.begin(115200);                            // Initialize serial communication at 9600
  Wire.begin();                                  // Initialize Arduino in I2C master.
}

void loop() 
{
  ChipID();                                       // Read Chip ID
  ManufacturerID();
  Diet(DieData);
  Objt();
  delay(1000);
}


void ChipID(void)
{
  Wire.beginTransmission(0x41);                  // I2C address of TMP006 = 0x41
  Wire.write(0xFF);                              // Setting the Pointer Register; Chip ID Register address 0xFF
  Wire.endTransmission();
  delay(100);
  
  Wire.requestFrom(0x41, 2);                     // Request 2 bytes data from TMP006
  uint16_t iData;
  while (Wire.available()) 
  { 
    iData = Wire.read();
    iData = iData<<8;
    iData |= Wire.read();
    Serial.print("Chip Identification Number: 0x");
    Serial.print(iData,HEX);                     // Print the received data
    Serial.println("");
  }
}

void ManufacturerID(void)
{
  Wire.beginTransmission(0x41);                  // I2C address of TMP006 = 0x41
  Wire.write(0xFE);                              // Setting the Pointer Register; Chip ID Register address 0xFF
  Wire.endTransmission();
  delay(100);
  
  Wire.requestFrom(0x41, 2);                     // Request 2 bytes data from TMP006
  uint16_t iData;
  while (Wire.available()) 
  { 
    iData = Wire.read();
    iData = iData<<8;
    iData |= Wire.read();
    Serial.print("Manufacturer Identification Number: 0x");
    Serial.print(iData,HEX);                     // Print the received data
    Serial.println("");
  }
}

void Diet(double &die)
{
  Wire.beginTransmission(0x41);                  // I2C address of TMP006 = 0x41
  Wire.write(0x01);                              // Setting the Pointer Register; Chip ID Register address 0xFF
  Wire.endTransmission();
  delay(100);
  
  Wire.requestFrom(0x41, 2);                     // Request 2 bytes data from TMP006
  uint16_t Data;
  while (Wire.available()) 
  { 
    Data = Wire.read();
    
    Data = Data<<8;
    Data |= Wire.read();
    Data >>= 2;
    die = Data;
    
    die *= 0.03125; // convert to celsius
    Serial.print("Die Temperature: ");
    Serial.print(die);                     // Print the received data
    Serial.println(" C");
  }
}

void Objt(void)
{
  Wire.beginTransmission(0x41);                  // I2C address of TMP006 = 0x41
  Wire.write(0x00);                              // Setting the Pointer Register; Chip ID Register address 0xFF
  Wire.endTransmission();
  delay(100);
  
  Wire.requestFrom(0x41, 2);                     // Request 2 bytes data from TMP006
  uint16_t iData;
  int16_t data;
  while (Wire.available()) 
  { 
    iData = Wire.read();
    iData = iData<<8;
    iData |= Wire.read();
    data = iData;
    double objData = data;
    objData = objData*156.2/1000000000; //Convert to V
    DieData += 273.15; //Convert to kelvin
    double T_m = DieData - T_ref;
    double S =  1 + (A_1 * T_m) + (A_2 * T_m * T_m);
    S = S*S_0/100000000000000;
    double V_os = B_0 + (B_1 * T_m) + (B_2 * T_m * T_m);
    double fV = ((objData-V_os) + (C_2*(objData-V_os)*(objData-V_os)));
    objData = sqrt(sqrt((DieData*DieData*DieData*DieData)+(fV/S)));
    objData -= 273.15; //Convert to celsius
    //DieData -= 273.15;
    Serial.print("Object Temperature: ");
    Serial.print(objData);                     // Print the received data
    Serial.println(" C\n");
  }
}
