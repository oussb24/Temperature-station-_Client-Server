/* ESP32 as Station */
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
# define MCP9808_I2C_ADDRESS 0x18
# define MCP9808_TEMPERATURE_REGISTER 0x05
# define MCP9808_CONFIG_REGISTER 0x01
# define MCP9808_CONFIG_SHUTDOWN 0x0100

uint8_t upperByte, lowerByte;
float temperature;
uint16_t conf_register;


String serverName = "http://192.168.11.1/";


long lastTime ;
long waitTime = 2000; // Waittime in ms

const char* ssid = "esp32network";
const char* password = "azerty00";

typedef enum {
  WAKEUP_MCP9808,
  READ_TEMP_MCP9808,
  SHUTDOWN_MCP9808,
  CALCULATE_TEMP,
  TRANSMIT_TEMP,
  WAIT
} AppState_t ;

AppState_t state = TRANSMIT_TEMP ;    

void setup () {
  Serial.begin (115200) ;
  Wire.begin (); // activate the i2c function on the micrcontroller
  initWiFi();
  transmitData();
}
void loop () {
  switch ( state ) {
    case WAKEUP_MCP9808 :
    wakeUp () ;
    state = READ_TEMP_MCP9808 ;
    break ;
    case READ_TEMP_MCP9808 :
    readTempReg () ;
    state = SHUTDOWN_MCP9808 ;
    break ;
    case SHUTDOWN_MCP9808 :
    shutDown () ;
    state = CALCULATE_TEMP ;
    break ;
    case CALCULATE_TEMP :
    calculateTemperature () ;
    Serial.println (" Temperature : " + String(temperature) + " *C") ;
    state = TRANSMIT_TEMP ;
    break ;
    case TRANSMIT_TEMP :
    transmitData () ;
    lastTime = millis ();
    state = WAIT ;
    break ;
    case WAIT :
    if ( millis () > waitTime + lastTime ) {
      state = WAKEUP_MCP9808 ;
    }
  }
}
void initWiFi() {
  WiFi.mode(WIFI_STA) ;
  WiFi.begin(ssid , password );
  Serial.print (" Connecting to WiFi ..") ;
  while ( WiFi.status() != WL_CONNECTED ){
    Serial.print(".");
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
}

void transmitData() {
  if( WiFi.status() == WL_CONNECTED ) {
    WiFiClient client;
    HTTPClient http;
    String urlStr = serverName + "?temperature="+String(temperature,2) ;
    http.begin(client , urlStr ) ;
// Send HTTP GET request
    int httpResponseCode = http.GET() ;
    if (httpResponseCode >0) {
      Serial.print(" HTTP Response code : ") ;
      Serial.println( httpResponseCode ) ;
      String payload = http.getString() ;
      Serial.println(payload) ;
    }
    http.end() ;
  }
}

//calculates temperature 
float calculateTemperature(){
  temperature =( upperByte & 0x0F) *16+ float ( lowerByte ) /16;
if (( upperByte & 0x10 ) == 0x10 ) { // temperature < 0 degree C
  temperature =256 - temperature ; // subtract from 2^8 since two ’scompliment
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
