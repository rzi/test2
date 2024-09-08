// Import required libraries
#include <FS.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "AsyncJson.h"

// Replace with your network credentials
const char* ssid = "RZI"; //Replace with your own SSID
const char* password = "rafalzietak"; //Replace with your own password

const int ledPin = 2;
String ledState;
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "GPIO_STATE"){
    if(digitalRead(ledPin)){
      ledState = "OFF";
    }
    else{
      ledState = "ON";
    }
    Serial.print(ledState);
    return ledState;
  }
  return String();
}

void setup(){
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  Serial.println("");
  Serial.println("Start");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
      Serial.println(dir.fileName());
      File f = dir.openFile("r");
      Serial.print( "rozmiar = ");
      Serial.println (f.size());
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  // Initialize a NTPClient to get time
  timeClient.begin(7200);
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(2);
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("get");
    //List all parameters
    int params = request->params();
    Serial.println( " params1 = "+ params);
    for (int i = 0; i < params; i++)
    {
    AsyncWebParameter* p = request->getParam(i);
    Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());     
    }
    request->send(SPIFFS, "/index.html", "text/html", false, processor); 
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/led2on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/led2off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
 
  server.onNotFound([](AsyncWebServerRequest *request) {
      request->send(404, "application/json", "{\"message\":\"Not found\"}");
  });
  
  // Start server
  server.begin();
}
 
void loop(){
  timeClient.update();
  int sek = timeClient.getSeconds();
  Serial.println(sek);
   
  if (sek ==0){
    time_t epochTime = timeClient.getEpochTime();
    Serial.print("Epoch Time: ");
    Serial.println(epochTime);
    
    String formattedTime = timeClient.getFormattedTime();
    Serial.print("Formatted Time: ");
    Serial.println(formattedTime);  

    int currentHour = timeClient.getHours();
    Serial.print("Hour: ");
    Serial.println(currentHour);  

    int currentMinute = timeClient.getMinutes();
    Serial.print("Minutes: ");
    Serial.println(currentMinute); 
    
    int currentSecond = timeClient.getSeconds();
    Serial.print("Seconds: ");
    Serial.println(currentSecond);  

    String weekDay = weekDays[timeClient.getDay()];
    Serial.print("Week Day: ");
    Serial.println(weekDay);    

    //Get a time structure
    struct tm *ptm = gmtime ((time_t *)&epochTime); 

    int monthDay = ptm->tm_mday;
    Serial.print("Month day: ");
    Serial.println(monthDay);

    int currentMonth = ptm->tm_mon+1;
    Serial.print("Month: ");
    Serial.println(currentMonth);

    String currentMonthName = months[currentMonth-1];
    Serial.print("Month name: ");
    Serial.println(currentMonthName);

    int currentYear = ptm->tm_year+1900;
    Serial.print("Year: ");
    Serial.println(currentYear);

    //Print complete date:
    String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
    Serial.print("Current date: ");
    Serial.println(currentDate);
    Serial.println("");
  }
  digitalWrite(ledPin, HIGH);
  delay(750);
  digitalWrite(ledPin, LOW);
}