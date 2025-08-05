#include "thingProperties.h"//include Arduino IoT cloud thing properties
#include <ArduinoHttpClient.h>//for sending data to ThingSpeak
#include "DHT.h"//dht sensor library
#define DHTPIN 2 //for digital pin connected to sensor
#define DHTTYPE DHT22 // for type of dht sensor
const char server[] = "api.thingspeak.com";//thing speak server
String apiKey = "FXE8P6O3JZ4WWUSQ";// api key

DHT dht(DHTPIN, DHTTYPE);//creation of sensor object

WiFiClient wifiClient;//creation of wifi client object
HttpClient client(wifiClient, server, 80);

unsigned long lastTime = 0;//stores the time of last data sent for the delay management

void setup() {
  Serial.begin(9600);//start serial communication
  delay(1500);//time for initialise

  dht.begin();//start sensor
  initProperties();  // cloud properties
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);//connection to cloud

// wait until connected
  while (ArduinoCloud.connected() == 0) {
    ArduinoCloud.update();//maintain connection updates
    delay(100);
  }

  Serial.println("Connected to Arduino IoT Cloud.");
}

void loop() {
  ArduinoCloud.update();//for keeping the connection
//data every 20 secs
  if (millis() - lastTime > 20000) {
    float temp = dht.readTemperature();//for temp in celsius
//if reading fails - print error
    if (isnan(temp)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      //succesfully read the temperature
      Serial.print("Sending temperasture: ");
      Serial.print(temp);
      Serial.println(" °C");

      String postData = "api_key=" + apiKey + "&field1=" + String(temp);
      //send the post request to thingspeak
      client.post("/update", "application/x-www-form-urlencoded", postData);
      //capture and print HTTP response status code and body
      int statusCode = client.responseStatusCode();
      String response = client.responseBody();
      Serial.print("HTTP Status: ");
      Serial.println(statusCode);
      Serial.print("Response: ");
      Serial.println(response);
    }
    lastTime = millis();//update the timestamp
  }
}