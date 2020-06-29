/*
 NodeMCU DEVKIT v1.0
 setup():
  Connect to LAN and get an IP address
  Connect to Firebase Realtime Database
 loop():
  Push luminosity value just digitalized
  Make LED blink once
  Wait for a delay before looping
*/





int sensorValue;                    // 10-bit equivalent value of analog signal

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <SPI.h>
#include <MFRC522.h>

int statuss = 0;
int out = 0;

#define SS_PIN 5  //D2
#define RST_PIN 4 //D1

#define WIFI_LED    D0  //GREEN 
#define OK_LED      D3 //RED;
#define WEB_LED   D4  //YELLOW
#define BUTTON A0 


#define WIFI_SSID "SLT-ADSL-17022"
#define WIFI_PASSWORD "ran2280252"

#define FIREBASE_HOST "esp-firebase-f918a.firebaseio.com"
#define FIREBASE_AUTH "eOF2m8Gywp0bVo0kNCpL9h5GWOnwu68zBe0VN9LA" // secret (40 chars)
#define DEVICE "device1" // Firebase Realtime Database node to store measures
#define INFO "device1/info" // Firebase Realtime Database node to store measures
#define  WEB_SWITCH "device1/web_switch"
#define  RED_LED "device1/red_led"


#define MEASURE_DELAY_SEC 3





byte ok_status = 0;
int web_status = 0 ;
int press_status = 0 ;
byte failCount = 0;

byte wifi_status = 0;
String content = "";
String id = "";

unsigned long readDelay = 2000;
unsigned long readTimer = 0 ;

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.


void setup() {
  Serial.begin(115200);
  

   
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  pinMode(WIFI_LED,OUTPUT);
  pinMode(OK_LED,OUTPUT);
  pinMode(WEB_LED,OUTPUT);
  

  allOff();

  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    blinkWifi();
  }


  
  allOff();
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Let's connect to Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

 firebaseErrorCheck();

  readRedLED();

}

byte green_led = 0;

void loop() {


  
reconnectWifi();


if(readWebSwitch()){
  WebLEDOn();
  while(readWebSwitch())delay(2000);
  allOff();
  }
   

if(readCard()){


 green_led = !green_led;
  if(green_led){
    greenLEDOn();
    }
  else{
    allOff();
    }   
   setInfo(id);
   saveRedLED();
   while(readCard())delay(1000);
  }

if(readButtonPress()){
   green_led = !green_led;
  if(green_led){
    greenLEDOn();
    }
  else{
    allOff();
    }   
   setInfo("button press"); 
   saveRedLED();

   while(readButtonPress())delay(1000);
  
  }   




delay(200);
}



void reconnectWifi(){
  
//  
//  if (WiFi.status() != WL_CONNECTED){
//
//  allOff();
//    
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//
//    blinkWifi();
//  }
//    allOff();
//
//  }
 if (WiFi.status() != WL_CONNECTED){
  Serial.println("wifi disconnected");
  ESP.restart();
 }


 
  }


void allOff(){
  
  
  digitalWrite(WIFI_LED,LOW);
  digitalWrite(OK_LED,LOW);
  digitalWrite(WEB_LED,LOW);
  
  }

void allOn(){
  
  
  digitalWrite(WIFI_LED,HIGH);
  digitalWrite(OK_LED,HIGH);
  digitalWrite(WEB_LED,HIGH);
  
  }  

void WebLEDOn(){
  
  
  digitalWrite(WIFI_LED,LOW);
  digitalWrite(OK_LED,LOW);
  digitalWrite(WEB_LED,HIGH);
  
  }

void greenLEDOn(){
  
  
  digitalWrite(WIFI_LED,LOW);
  digitalWrite(OK_LED,HIGH);
  digitalWrite(WEB_LED,LOW);
  
  }   

 void redLEDOn(){
  
  
  digitalWrite(WIFI_LED,HIGH);
  digitalWrite(OK_LED,LOW);
  digitalWrite(WEB_LED,LOW);
  
  }   

void blinkWifi(){
    wifi_status = !wifi_status;
  digitalWrite(WIFI_LED,wifi_status);
  }

boolean readWebSwitch(){
  
if(readTimer > millis()) return web_status?true:false; 

   readTimer = millis() + readDelay;
//   Serial.println("millis ok");
   web_status = Firebase.getInt(WEB_SWITCH);
  // handle error
  // firebaseErrorCheck();
  byte  failed = 0;
      Serial.println(Firebase.failed()); 


  if (failed && failCount > 3){
//      allOn();
      Serial.print("setting /message failed:");
      Serial.println(Firebase.error()); 
      ESP.restart();
  }
  else if(failed && failCount < 3){
         failCount++;
         Serial.println("firebase failing");
    }
  else{
     failCount = 0;
    }
    
//   Serial.println(web_status);
   return web_status?true:false; 
    }

boolean readButtonPress(){

if(analogRead(A0)>600){
  press_status = 1 ;

}
else{
  press_status = 0 ;
  
  }
  return press_status?true:false;
}


 
void readRedLED(){
     green_led = Firebase.getInt(RED_LED);

     if(green_led){
      greenLEDOn();
      }  

      else{
            allOff();
        }
        
    firebaseErrorCheck();
  }

void saveRedLED(){
   Firebase.setInt(RED_LED,green_led);
   firebaseErrorCheck();
  }

void  setInfo(String infor){
  
    Firebase.setString(INFO, infor);
    firebaseErrorCheck();
}

void firebaseErrorCheck(){
  if (!Firebase.failed())return;
//      allOn();
      Serial.print("setting /message failed:");
      Serial.println(Firebase.error()); 
      delay(1000); 
      ESP.restart();
  
}


 

boolean readCard() {
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return false;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return false;
  }

  content = "";
  id = "";
              byte letter;
              for (byte i = 0; i < mfrc522.uid.size; i++)
              {
              content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
              content.concat(String(mfrc522.uid.uidByte[i], HEX));
            }

              id = content.substring(1);

              Serial.println(id);
               return true;
            }



  
