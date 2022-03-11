/*************************************
 * Author: Cristian Capraro
 * Date: 15-07-2021
 * Revision Date: 11-03-2022
 * Test Suite for Arduino MKR 1010
 * MIT LICENCE - 2022
 *************************************/

#include <WiFiNINA.h>
#include <ArduinoBLE.h>


//WiFi Config
#define RETRY 3
char _SSID[] = "ChangeThisWithYourWiFiName";
char _PASS[] = "ChangeThisWithYourWiFiPassword";
int conn_state = WL_IDLE_STATUS;
int numSsid;


//Bluetooth Config
char DEVICE_NAME[] = "Arduino DevBoard (Debug)";
bool BLEstate = false;


//RGB Config (OnBoard)
#define R_LED 25
#define G_LED 26
#define B_LED 27


//External pin Config
#define PWM_LED 3
#define BTN_PIN 7 //WARNING: PINMODE FOR THIS PIN IS "INPUT_PULLUP"


//Other Config
char serial_input;


//Serial Welcome message
void WelcomeMessage(){
  Serial.println("|        WELCOME!            |");
  Serial.println("|Press 'h' for help          |");
  Serial.println("|Press 's' to show pin config|");
  Serial.println("|Press 'c' to connect WiFi   |");
  Serial.println("|Press 'l' to list WiFi conn.|");
  Serial.println("|Press 'd' to disconnect WiFi|");
  Serial.println("|Press 'v' for version       |");
  Serial.println("|Press 'f' for led ctrl      |");
  Serial.println("|Press 't' to test ext led   |");
  Serial.println("|Press 'b' to test button    |");
  Serial.println("|Press 'k' to start BLE      |");
  Serial.println("|Press 'j' to stop  BLE      |");
  }


//RGB OnBoard functions
void fade(int led_n){
  Serial.println("Phase 0: Switch off");
  WiFiDrv::digitalWrite(led_n, LOW); //Switch off
  Serial.println("Phase 1: 0 --> 255");
  for(int i = 0; i < 256; i++){
    WiFiDrv::analogWrite(led_n, i);
    delay(5);
  }
  delay(100);
  Serial.println("Phase 2: 255 --> 0");
  for(int i = 255; i >= 0; i--){
    WiFiDrv::analogWrite(led_n, i);
    delay(5);
  }
}

void fadeAllRGB(){
  fade(R_LED);
  delay(50);
  fade(G_LED);
  delay(50);
  fade(B_LED);
}


//BLE functions
void startAdvBLE(char n[]){
    //Bluetooth init
    BLE.begin();
    BLE.setLocalName(n); //Set public BLE name
    BLE.advertise();
    Serial.println("BLE service started successfully!");
    Serial.println("Connection Name: "+(String)n);
  }

void stopBLE(){
  BLE.end();
  Serial.println("BLE service stopped successfully!");
  }


//WiFi functions
void cnWiFi(char * ssid, char * pass){ //Connect
  for(int i = 1; i <= RETRY && conn_state != WL_CONNECTED; i++){
    Serial.println("Wait...(Attemp: "+(String)i+" of "+(String)RETRY+")");
    conn_state = WiFi.begin(ssid, pass);
  }
  //Check
  if(conn_state == WL_CONNECTED){ Serial.println("SUCCESSFULLY CONNECTED!"); digitalWrite(LED_BUILTIN, HIGH); }
  else Serial.println("CONNECTION ERROR!");
}

void dcWiFi(){ //Disconnect
  if(conn_state == WL_CONNECTED){ conn_state = WiFi.disconnect(); digitalWrite(LED_BUILTIN, LOW); delay(100); Serial.println("SUCCESSFULLY DISCONNECTED"); }
  else Serial.println("ALREADY DISCONNECTED");
}

void scanWiFi(){ //Scan for WiFi AP
  Serial.println("Scanning for WiFi AP, Please wait...");
  numSsid = WiFi.scanNetworks();
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet); //Lateral number
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet)); //SSID
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet)); //Signal
    Serial.print(" dBm");
    Serial.println();
  }
  Serial.println("Scan Completed");
}


//External Pin Functions
void printPinInfo(){ //Print All Pin Info
  Serial.println("|      PIN CONFIG      |");
  Serial.println("|BUTTON PIN: "+(String)BTN_PIN+"         |");
  Serial.println("|LED PIN:    "+(String)PWM_LED+"         |");
  Serial.println("........................");
  Serial.println("|   RGB CONFIG(WiFi)   |");
  Serial.println("|R PIN:     "+(String)R_LED+"         |");
  Serial.println("|G PIN:     "+(String)G_LED+"         |");
  Serial.println("|B PIN:     "+(String)B_LED+"         |");
  }


//START HERE:
void setup() {
  //Serial init
  Serial.begin(9600);
  
  pinMode(PWM_LED, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  while(!Serial);//Wait for serial connection
  WelcomeMessage();
}


void loop() {
  if(Serial.available() > 0){
      serial_input = Serial.read();
      if(serial_input != 10){
          for(int i = 0; i < 3; i++) Serial.println("");
          switch(serial_input){
              case 'h': //Help
                WelcomeMessage();
                break;
              case 's': //Show PIN Config:
                printPinInfo();
                break;
              case 'c': //Connect
                cnWiFi(_SSID, _PASS);
                break;
              case 'd': //Disconnect
                dcWiFi();
                delay(1000);
                break;
              case 'l': //List All WiFi AP in Area
                scanWiFi();
                break;
              case 'v': //Version Info
                Serial.println("Firmware Version: "+(String)WiFi.firmwareVersion());
                break;
              case 'f': //RGB led test
                fadeAllRGB();
                break;
              case 't': //Extenal LED Test (Remember, Arduino MKR WiFi 1010 works with 3.3V Only)
                digitalWrite(PWM_LED, LOW);
                for(int i = 0; i < 256; i++){ analogWrite(PWM_LED, i); delay(10);}
                delay(250);
                for(int i = 255; i >= 0; i--){ analogWrite(PWM_LED, i); delay(10);}
                delay(250);
                Serial.println("DONE!");
                break;
              case 'b': //Test button (internal pullup)
                Serial.println("BTN MODE ON");
                Serial.println("Press 'e' to exit");
                while(true){
                  if(digitalRead(BTN_PIN) == LOW) analogWrite(PWM_LED, 255);
                  else analogWrite(PWM_LED, 0);
                  if(Serial.read() == 'e') break;
                }
                Serial.println("BTN MODE OFF");
                break;
              case 'k': //Start BLE
                if(BLEstate == false){
                  startAdvBLE(DEVICE_NAME);
                  BLEstate = true;
                  }
                else Serial.println("BLE is started!");
                break;
              case 'j': //Stop BLE
                if(BLEstate == true){
                  stopBLE();
                  BLEstate = false;
                  }
                else Serial.println("BLE is stopped!");
                break;
              default: //Error
                Serial.println("Invalid Command!");
                break;
            }
        }
    }
}
