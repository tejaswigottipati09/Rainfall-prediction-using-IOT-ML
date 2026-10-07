#include <ESP8266WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define pins and settings
#define DHTPIN D4          // DHT11 data pin
#define DHTTYPE DHT11
#define LDR_PIN A0         // LDR connected to analog pin A0

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
char ssid[] = "purple";   // your network SSID (name) 
char pass[] = "1234567890";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = 3019107;
const char * myWriteAPIKey = "9T3U9RKNY82FH3FX";

// Initialize our values
int number1 = 0;
int number2 = random(0,100);
int number3 = random(0,100);
int number4 = random(0,100);
String myStatus = "";

void setup() {
  Serial.begin(115200);  // Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  dht.begin();
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(2000);
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
   float temp = dht.readTemperature();
  float humid = dht.readHumidity();

  // Read LDR value
  int ldrValue = analogRead(LDR_PIN);


  // Print to Serial
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print(" C  Humid: ");
  Serial.print(humid);
  

  // Print to LCD
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temp, 1);
  lcd.print("C H:");
  lcd.print(humid, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Light: ");
  lcd.print(ldrValue);
  lcd.print(" %");


  // set the fields with the values
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, humid);
  ThingSpeak.setField(3, ldrValue);
 

  // figure out the status message
  if(number1 > number2){
    myStatus = String("field1 is greater than field2"); 
  }
  else if(number1 < number2){
    myStatus = String("field1 is less than field2");
  }
  else{
    myStatus = String("field1 equals field2");
  }
  
  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  // change the values
  number1++;
  if(number1 > 99){
    number1 = 0;
  }
  number2 = random(0,100);
  number3 = random(0,100);
  number4 = random(0,100);
  
  delay(20000); // Wait 20 seconds to update the channel again
}