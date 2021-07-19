#include <Wire.h>
# define MCP9808_I2C_ADDRESS 0x18
# define MCP9808_TEMPERATURE_REGISTER 0x05
# define MCP9808_CONFIG_REGISTER 0x01
# define MCP9808_CONFIG_SHUTDOWN 0x0100
uint8_t upperByte, lowerByte;
float temperatur;
uint16_t conf_register;

void setup() {
  Serial.begin (115200);
  while (! Serial );
Wire.begin (); // activate the i2c function on the micrcontroller
}
void loop () {
  wakeUp();
  readTempReg();
  shutDown();
  calculateTemp(upperByte,lowerByte);
  Serial.print ( temperatur ,4);
  Serial.println ("*C");
  delay (2000) ;

  
}

//calculates temperature 
float calculateTemp(uint8_t upperByte, uint8_t lowerByte){
  temperatur =( upperByte & 0x0F) *16+ float ( lowerByte ) /16;
if (( upperByte & 0x10 ) == 0x10 ) { // temperature < 0 degree C
  temperatur =256 - temperatur ; // subtract from 2^8 since two ’scompliment
}
  
}

void readTempReg(){
  
  // set pointer to temperature register
  Wire.beginTransmission( MCP9808_I2C_ADDRESS );
  Wire.write ( MCP9808_TEMPERATURE_REGISTER );
  Wire.endTransmission ();
//2 Bytes reading
  Wire.requestFrom( MCP9808_I2C_ADDRESS , 2);
  if (2 <= Wire.available () ) {
   upperByte = Wire.read();
   lowerByte = Wire.read();
  }
}

void shutDown(){
  conf_register|= MCP9808_CONFIG_SHUTDOWN;
}
void wakeUp(){
  conf_register &= ~MCP9808_CONFIG_SHUTDOWN;
}
