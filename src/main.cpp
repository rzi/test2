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
#include <EEPROM.h>
#include <EDB.h>

// #include <ESP_EEPROM.h>

// Replace with your network credentials
const char* ssid = "RZI"; //Replace with your own SSID
const char* password = "rafalzietak"; //Replace with your own password

const int ledPin = 2;
const int inputPin = 4;
String ledState;
int triger =0;
bool isOn =false;
int current  =0;
int startTime =0;
int timeON =0;
int timeOFF =0; 
int stopwatchStatus, stopwatchStart, stopwatchStop;
// unsigned long miganieLED1 = 2000;
// unsigned long aktualnyCzas = 0;
// unsigned long zapamietanyCzasLED1 = 0;
unsigned long previousMillis = 0;  // Zmienna do przechowywania poprzedniego czasu
long intervalOn = 1000;  // Czas włączenia diody (1 sekunda = 1000 ms)
long intervalOff = 2000; // Czas, po którym wyłączy się dioda (2 sekundy = 2000 ms)

IPAddress local_IP(192, 168, 100, 7);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // opcjonalnie
IPAddress secondaryDNS(8, 8, 4, 4); // opcjonalnie

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

#define EEPROM_SIZE 512           // Rozmiar EEPROM (dla ESP8266)
#define RECORD_SIZE sizeof(LogRecord) // Wielkość jednego rekordu
#define MAX_RECORDS (EEPROM_SIZE / RECORD_SIZE) // Maksymalna liczba rekordów
JsonDocument doc;
// Struktura przechowująca dane, które chcemy zapisywać
struct LogRecord {
  int type;
  int year;
  int month;
  int day;
  int hour;
  int minute;
};
// Inicjalizacja EEPROM
void initEEPROM() {
  EEPROM.begin(EEPROM_SIZE);     // Rozpoczynamy pracę z EEPROM
}
// Zmienna śledząca ostatnią zapisaną pozycję w EEPROM
int currentRecordIndex = 0;
// Funkcja zapisu nowego rekordu do EEPROM
void saveRecord(LogRecord record) {
  // Sprawdzamy, czy jesteśmy w stanie zapisać kolejny rekord
  if (currentRecordIndex < MAX_RECORDS) {
    // Obliczamy adres, pod który zapisujemy dane
    int address = currentRecordIndex * RECORD_SIZE;
    EEPROM.put(address, record);  // Zapisujemy rekord
    EEPROM.commit();              // Potwierdzamy zapis do pamięci Flash
    currentRecordIndex++;         // Przesuwamy wskaźnik na kolejny rekord
    Serial.println("Rekord zapisany.");
  } else {
    Serial.println("EEPROM pełne, nie można zapisać więcej danych.");
  }
}
// Funkcja odczytu wszystkich zapisanych rekordów z EEPROM
void readAllRecords() {
  LogRecord record;
  for (int i = 0; i <= MAX_RECORDS; i++) {
    int address = i * RECORD_SIZE;
    EEPROM.get(address, record);  // Odczytujemy rekord
    Serial.print("Rekord ");
    Serial.print(i);
    Serial.print(": Typ: ");
    Serial.print(record.type);
    Serial.print(", Year: ");
    Serial.print(record.year);
    Serial.print(", Month: ");
    Serial.print(record.month);
    Serial.print(", Day: ");
    Serial.print(record.day);
    Serial.print(", Hour: ");
    Serial.print(record.hour);
    Serial.print(", Minute: ");
    Serial.println(record.minute);
  }
}
// Funkcja wyczyszczenia EEPROM (reset)
void clearEEPROM() {
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0xFF);  // Zapisujemy 0xFF (czyli "pusty" EEPROM)
  }
  EEPROM.commit();
  currentRecordIndex = 0;
  Serial.println("EEPROM wyczyszczony.");
}

void setup(){
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(inputPin, INPUT);
  digitalWrite(ledPin, HIGH); 
  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  Serial.println("");
  Serial.println("Start");
  Serial.print("input pin = ");
  Serial.println(digitalRead(inputPin));
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
      Serial.println(dir.fileName());
      File f = dir.openFile("r");
      Serial.print( "rozmiar = ");
      Serial.println (f.size());
  }
  // Connect to Wi-Fi
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
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
  LogRecord record;
  // Obsługa favicon
  server.on("/favicon2.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/favicon2.png", "image/png");
    Serial.println("favicon");
    request->send(response);
  });
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("get");
    //List all parameters
    int params = request->params();
    String year;        
    String month;
    String day;
    String hour;
    String minute;

    for (int i = 0; i < params; i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      // format 2024-09-01 12:00
        String name =String(p->name().c_str());
        String value = String(p->value().c_str());
        Serial.print( "name ");
        Serial.println( name);
        Serial.print( "value ");
        Serial.println( value);
        for (i=0 ;i<4 ;i++){
          year = year + p->value().charAt(i);
        } 
        Serial.println(year);
        month = p->value().charAt(5) ;
        month = month + p->value().charAt(6);
        Serial.println(month);
        day = p->value().charAt(8) ;
        day = day + p->value().charAt(9);
        Serial.println(day);
        hour = p->value().charAt(11) ;
        hour = hour + p->value().charAt(12);
        Serial.println(hour);
        minute = p->value().charAt(14) ;
        minute = minute + p->value().charAt(15);
        Serial.println(minute);
      if (String(p->name().c_str()) == "start"){
      LogRecord record1 = { 1, year.toInt(), month.toInt(), day.toInt(), hour.toInt(), minute.toInt()};
      saveRecord(record1); 
      } else{
      LogRecord record1 = { 0, year.toInt(), month.toInt(), day.toInt(), hour.toInt(), minute.toInt()};
      saveRecord(record1); 
      }   
    }
    Serial.print("currentRecordIndex ");
    Serial.println(currentRecordIndex );
    request->send(SPIFFS, "/index.html", "text/html", false, processor); 
  });
  server.on("/program", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("get program");
    //List all parameters
    int params = request->params();
    String year;        
    String month;
    String day;
    String hour;
    String minute;

    for (int i = 0; i < params; i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      // format 2024-09-01 12:00
        String name =String(p->name().c_str());
        String value = String(p->value().c_str());
        Serial.print( "name ");
        Serial.println( name);
        Serial.print( "value ");
        Serial.println( value);
        for (i=0 ;i<4 ;i++){
          year = year + p->value().charAt(i);
        } 
        Serial.println(year);
        month = p->value().charAt(5) ;
        month = month + p->value().charAt(6);
        Serial.println(month);
        day = p->value().charAt(8) ;
        day = day + p->value().charAt(9);
        Serial.println(day);
        hour = p->value().charAt(11) ;
        hour = hour + p->value().charAt(12);
        Serial.println(hour);
        minute = p->value().charAt(14) ;
        minute = minute + p->value().charAt(15);
        Serial.println(minute);
      if (String(p->name().c_str()) == "start"){
      LogRecord record1 = { 1, year.toInt(), month.toInt(), day.toInt(), hour.toInt(), minute.toInt()};
      saveRecord(record1); 
      } else{
      LogRecord record1 = { 0, year.toInt(), month.toInt(), day.toInt(), hour.toInt(), minute.toInt()};
      saveRecord(record1); 
      }   
    }
    Serial.print("currentRecordIndex ");
    Serial.println(currentRecordIndex );
    request->send(SPIFFS, "/program.html", "text/html", false, processor); 
  });
  server.on("/stopwatch", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("get stopwatch");
    //List all parameters
    int params = request->params();
    Serial.print("params = ");
    Serial.println(params);

    for (int i = 0; i < params; i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      Serial.printf("GET %s %s\n", p->name().c_str(), p->value().c_str());
        String start =String(p->name().c_str());
        String stop = String(p->value().c_str());
        Serial.print( "start ");
        Serial.println( start);
        Serial.print( "stop ");
        Serial.println(stop);
    }
    request->send(SPIFFS, "/stopwatch.html", "text/html", false, processor); 
  });
  server.on("/stopwatch2", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("get stopwatch2");
    int params = request->params();
    Serial.print("params = ");
    Serial.println(params);
    // int status;
    // int start;
    // int stop;

    for (int i = 0; i < params; i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      Serial.printf("GET %s %s \n", p->name().c_str(), p->value().c_str());
        String key =String(p->name().c_str());
        String value = String(p->value().c_str());

        Serial.print( "key ");
        Serial.println( key);
        Serial.print( "value ");
        Serial.println(value);
        Serial.println("------.....");
        if (key == "start") {
          stopwatchStart = value.toInt();
        } else if( key =="stop"){
          stopwatchStop =value.toInt();
          digitalWrite(ledPin, HIGH); 
        }else if (key == "status")
        {
          stopwatchStatus = value.toInt();
        }else if (key == "mycheckbox")
        {
          int val = value.toInt();
          if (val == 1){
            triger =1;
            digitalWrite(ledPin, HIGH); 
          } else{
            triger =0;
            
          }
          stopwatchStatus = 0; 
        }else  
        {
           Serial.print("nieznana wartosc = ");
           Serial.println(stopwatchStatus);
        }
    }
    request->send(200, "application/json", String(stopwatchStatus));
  });
  server.on("/manual", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("get manual");
    //List all parameters
    int params = request->params();
    Serial.print("params = ");
    Serial.println(params);

    for (int i = 0; i < params; i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      Serial.printf("GET %s %s\n", p->name().c_str(), p->value().c_str());
        String start =String(p->name().c_str());
        String stop = String(p->value().c_str());
        Serial.print( "start ");
        Serial.println( start);
        Serial.print( "stop ");
        Serial.println(stop);
    }
    request->send(SPIFFS, "/manual.html", "text/html", false, processor); 
  });
  server.on("/manual2", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("get manual2");
    int params = request->params();
    Serial.print("params = ");
    Serial.println(params);
    int status;
    int start;
    int stop;

    for (int i = 0; i < params; i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      Serial.printf("GET %s %s \n", p->name().c_str(), p->value().c_str());
        String key =String(p->name().c_str());
        String value = String(p->value().c_str());

        Serial.print( "key ");
        Serial.println( key);
        Serial.print( "value ");
        Serial.println(value);
        Serial.println("------");
        if (key == "start") {
          start = value.toInt();
        } else if( key =="stop"){
          stop =value.toInt();
        }else if (key == "status")
        {
          status = value.toInt();
        }else  
        {
           Serial.println("nieznana wartosc");
        }
    }
    request->send(200, "application/json", String(status));
  });
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/normalize.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/normalize.css", "text/css");
  });
  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/main.js", "text/html");
  });
  server.on("/manual.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/manual.js", "text/html");
  });
  server.on("/stopwatch.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/stopwatch.js", "text/html");
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
  server.on("/eeprom", HTTP_GET, [](AsyncWebServerRequest *request){
    LogRecord record;
    String output;
    for (int i = 0; i < MAX_RECORDS; i++) {
      int address = i * RECORD_SIZE;
      EEPROM.get(address, record);  // Odczytujemy rekord
      // String myDoc;
      // myDoc = myDoc+String(i);
      JsonObject myDoc = doc.createNestedObject(i);
      // Serial.print("Rekord ");
      // Serial.print(i);
      // Serial.print(": Typ: ");
      // Serial.print(record.type);
      myDoc["type"] = record.type;
      // Serial.print(", Year: ");
      // Serial.print(record.year);
      myDoc["year"] = record.year;
      // Serial.print(", Month: ");
      // Serial.print(record.month);
      myDoc["month"] = record.month;
      // Serial.print(", Day: ");
      // Serial.print(record.day);
      myDoc["day"] =record.day;
      // Serial.print(", Hour: ");
      // Serial.print(record.hour);
      myDoc["hour"] = record.hour;
      // Serial.print(", Minute: ");
      // Serial.println(record.minute);
      myDoc["minute"] = record.minute;
      
      serializeJson(doc, output);
    }
    // Serial.print("json output: ");
    // Serial.println(output);
    request->send(200, "application/json", output);
  });
  server.on("/del", HTTP_GET, [](AsyncWebServerRequest *request){
    String myIndexString;
    Serial.println("get del");
    //List all parameters
    int params = request->params();
    byte myArry[EEPROM_SIZE];

    for (int i = 0; i < EEPROM_SIZE; i++) {
      EEPROM.get(i, myArry[i]);  // Odczytujemy rekord
    }
    for (int i = 0; i < params; i++){
      AsyncWebParameter* p = request->getParam(i);
      Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      String name =String(p->name().c_str());
      String value = String(p->value().c_str());
      if (String(p->name().c_str()) == "index"){
        myIndexString = p->value().charAt(0) ;
        int myIndex = myIndexString.toInt();
        Serial.print( "MAX_RECORDS ");
        Serial.println(MAX_RECORDS);
        Serial.print( "RECORD_SIZE ");
        Serial.println(RECORD_SIZE);
        Serial.print( "myIndex ");
        Serial.println(myIndex);
        for (int j=0 ;j< EEPROM_SIZE ;j++){
          if (myIndex == 0){
            if (j <  EEPROM_SIZE-RECORD_SIZE){
            myArry[j] = myArry[j+ RECORD_SIZE];
            } else {
              myArry[j] = 0xFF;
            }
          }else{
            Serial.print( " myArry[j] = ");
            Serial.print(myArry[j]);
            if (j <  EEPROM_SIZE-RECORD_SIZE){
              int ind = myIndex* RECORD_SIZE;
              if (j >= ind)
               myArry[j] = myArry[j+RECORD_SIZE];
            }
            else  {
              myArry[j] = 0xFF;
            }
          }
        }
      } else{
        Serial.print( "błąd ");
      }   
    }
    for (int i = 0 ; i < EEPROM_SIZE; i++) {
          EEPROM.write(i, myArry[i]);                 
    }
    EEPROM.commit();
    if (currentRecordIndex >0 ) {
        currentRecordIndex =currentRecordIndex -1;
    }
    Serial.print("currentRecordIndex " );
    Serial.println(currentRecordIndex );
    request->send(SPIFFS, "/index.html", "text/html", false, processor); 
  });
  server.on("/erase", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("get erase");
    clearEEPROM();
    request->send(SPIFFS, "/index.html", "text/html", false, processor); 
  });

  // Start server
  server.begin();
  initEEPROM();                    // Inicjalizujemy EEPROM
  readAllRecords();
}
void loop(){

  timeClient.update();
  int sek = timeClient.getSeconds();
  // Serial.println(sek);
  unsigned long currentMillis = millis();  // Aktualny czas
  int readPin =digitalRead(inputPin);
  intervalOn = stopwatchStart * 1000;
  intervalOff = stopwatchStop * 1000;

  // Serial.printf("readpin %i  triger %i isOn %d status %i start %i stop %i current %i previous %i diff %i\n", 
  // readPin,triger,isOn,stopwatchStatus, stopwatchStart,stopwatchStop, currentMillis, previousMillis , diff);
    // Serial.printf("readpin %i  triger %i isOn %d status %i start %i stop %i current %i intervalOn %i intervalOff %i\n", 
  // readPin,triger,isOn,stopwatchStatus, stopwatchStart,stopwatchStop, current, intervalOn , intervalOff);
  if(stopwatchStatus == 1 ){
    // Jeśli dioda jest wyłączona, zaświeć ją na 1 sekundę
    if (digitalRead(ledPin) == LOW && currentMillis - previousMillis >= intervalOn) {
      previousMillis = currentMillis;   // Zaktualizuj poprzedni czas
      digitalWrite(ledPin, HIGH);       // Włącz diodę          
    }
    // Jeśli dioda jest włączona, wyłącz ją po 2 sekundach
    if (digitalRead(ledPin) == HIGH && currentMillis - previousMillis >= intervalOff) {
      previousMillis = currentMillis;   // Zaktualizuj poprzedni czas
      digitalWrite(ledPin, LOW);        // Wyłącz diodę        
    }
  }

    // Serial.print("ledPin = ");
    // Serial.print(digpinPinitalRead(ledPin));  
    Serial.print(" readPin = ");
    Serial.println(readPin); //sprawdzi czy readPin powinno by 0
   
    if (!isOn && triger == 1 && readPin == 0) {  
      isOn = true;                
      startTime = millis();         
      timeON = startTime + intervalOn;
      timeOFF  = startTime + intervalOn+intervalOff;
      // Serial.print(" startTime = ");
      // Serial.print(startTime);  
      // Serial.print(" timeON = ");
      // Serial.print(timeON); 
      // Serial.print(" timeOFF = ");
      // Serial.println(timeOFF); 
    }
    current = millis();
    // Serial.print(" startTime = ");
    // Serial.print(startTime);  
    // Serial.print("current = ");
    // Serial.println(current);  
    // Serial.print(" intervalOn = ");
    // Serial.println(intervalOn);  
    if (triger == 1 &&( current < timeON)) { 
      digitalWrite(ledPin, LOW);
    } else if (triger == 1 && ( current >= timeON && current <= timeOFF ))
    {
      digitalWrite(ledPin, HIGH);
    }else{
      isOn = false;
    }
    
    delay(500);

  if (sek == 0 ){
    time_t epochTime = timeClient.getEpochTime();
    Serial.print("Epoch Time: ");
    Serial.println(epochTime);
    
    String formattedTime = timeClient.getFormattedTime();
    Serial.print("Formatted Time: ");
    Serial.println(formattedTime);  

    int currentHour = timeClient.getHours()+2;
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

    Serial.print("Current index: ");
    Serial.println(currentRecordIndex);

    Serial.println("Odczyt eeprom");   
    // readAllRecords();

    // RTC
     LogRecord record[MAX_RECORDS];
      for (int i = 0; i < MAX_RECORDS; i++) {
        int address = i * RECORD_SIZE;
        EEPROM.get(address, record[i]);  // Odczytujemy rekord
        Serial.print("Rekord ");
        Serial.print(i);
        Serial.print(": Typ: ");
        Serial.print(record[i].type);
        Serial.print(", Year: ");
        Serial.print(record[i].year);
        Serial.print(", Month: ");
        Serial.print(record[i].month);
        Serial.print(", Day: ");
        Serial.print(record[i].day);
        Serial.print(", Hour: ");
        Serial.print(record[i].hour);
        Serial.print(", Minute: ");
        Serial.println(record[i].minute);
         if (currentYear == record[i].year && currentMonth == record[i].month && monthDay == record[i].day &&
         currentHour == record[i].hour && currentMinute == record[i].minute && record[i].type==1){
          //set
          Serial.println("załączeine"); 
          digitalWrite(ledPin, LOW);  
          // delay(750);
        }else if (currentYear == record[i].year && currentMonth == record[i].month && monthDay == record[i].day &&
          currentHour == record[i].hour && currentMinute == record[i].minute && record[i].type == 0){
        // unset
        digitalWrite(ledPin, HIGH); 
        Serial.println("wyłączeine"); 
        } 
        if (record[i].type >=0 && record[i].type <2 ){
          Serial.print("record H: ");
          Serial.print(record[i].hour);
          Serial.print(" minute M: ");
          Serial.println(record[i].minute);
          Serial.print("current H: ");
          Serial.print(currentHour);
          Serial.print(" current M: ");
          Serial.println(currentMinute);
        }
      }
  }
  // delay(750);
}