# Temperature-station-_Client-Server
This project is a temperature station usingtwo ESP32 boards. Board A is conneected to a temperature sensor (MCP9808) and sends the temperature measurements to Board B each 2 second.s
First the connection between the two boards is established via WIFI, then the temperature is sent from the client to the server via an HTTP GET request. 
The temperature sensor is set to "sleep" after each temperature measurement in order to save energy. It "wakes up" right before taking a new measurement.
