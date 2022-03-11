/*****************************
 * Author: Cristian Capraro  *
 * Novara - Piedmont, Italy  *
 * Version: 1.0 Revised      *
 * Wireless Connection Test  *
 * Date:     24-08-2021      *
 * R. Date:  11-03-2022      *
 *   Board: MKR WiFi 1010    *
 *****************************/
//Libraries
#include <WiFiNINA.h>

//Macro

//Program Macro
#define SHOW_CREDENTIAL //Optional
  
//WiFi Connection Macro
#define RETRY_MAX 3 //--> Retry to connect -- *note: stardard delay(10000);

//RGB OnBoard Pin / Macro
#define onBoardRGB(led, val) WiFiDrv::analogWrite(led, val)
#define RLED 25 // --> Red
#define GLED 26 // --> Green
#define BLED 27 // --> Blue

//Variables
  //WiFi
  String wssid, wpass;
  int wstate = WL_IDLE_STATUS;
  
  //Program
  char confirmx; //Used to confirm data
  char selectx; //Choice char
  
void setup() {
   Serial.begin(9600);
   Serial.setTimeout(1); //TimeOut for "Serial.readString();"
   while(!Serial); //Wait Serial Terminal
}

void loop() {
  WIFICONFIG: //Label [1]
  Serial.println("Waiting Credential");
  Serial.println("Insert WIFI SSID: ");
  while(!(Serial.available() > 0)); //Wait user input
  wssid = Serial.readString(); //Read String on Serial
  delay(500);
  Serial.println("Insert WIFI PASSWORD: ");
  while(!(Serial.available() > 0)); //Wait user input
  wpass = Serial.readString(); //Read String on Serial
  delay(500);
  const char * ssid = wssid.c_str(); //Convert String to char array
  const char * pass = wpass.c_str(); //Convert String to char array

  //Confirm Data Sent
  CONFIRM: //Label [2]
  Serial.println("Confirm Y/N? ");
  while(!(Serial.available() > 0));
  confirmx = Serial.read();
  if(confirmx == 'N'){Serial.println("OK...No problem! :D"); goto WIFICONFIG;}
  else if(confirmx == 'Y'){Serial.println("OK...Let's Go! :D");}
  else{Serial.println("...I don't Understand what you want! :-( "); goto CONFIRM;};
  
  //Print inserted SSID / PASSWORD
  #ifdef SHOW_CREDENTIAL
  Serial.println(ssid);
  Serial.println(pass);
  #endif
  
  connettiWiFi(ssid, pass); //Try to connect

  //Loop Function:
  while(true){ //Loop:
    if(Serial.available() > 0){ 
      selectx = Serial.read();
          switch(selectx){
          case 'd': //Disconnect WiFi and Re-Do configuration
              Serial.println("Re-Config...");
              if(wstate == WL_CONNECTED){ 
                wstate = WiFi.disconnect(); //Disconnect WiFi
                for(int i = 0; i < 256; i++){onBoardRGB(RLED, i); delay(4);}
                for(int i = 255; i >= 0; i--){onBoardRGB(RLED, i); delay(4);}
                Serial.println("Disconnected...");
              }
              goto WIFICONFIG; //--> go to Label
              break;
          default:
              Serial.println("Invalid Command");
      }
    }
  }
}

void connettiWiFi(const char * myssid, const char * mypass){
    for(int i = 0; wstate != WL_CONNECTED && i < RETRY_MAX; i++){
       onBoardRGB(RLED, 160); onBoardRGB(GLED, 160);
       Serial.println("Attemp "+String(1+i)+" of "+((int)(RETRY_MAX)));
       wstate = WiFi.begin(myssid, mypass); delay(10000);
      };
    Serial.println(wstate == WL_CONNECTED ? "Connection OK" : "Connection KO"); //Verify Wireless Connection
    wstate == WL_CONNECTED ? onBoardRGB(RLED, 0) : onBoardRGB(GLED, 0);
    if(wstate == WL_CONNECTED){Serial.println("Arduino IP Address: "); Serial.print(WiFi.localIP()); Serial.println("");};
    delay(2000);
    onBoardRGB(RLED, 0); onBoardRGB(GLED, 0);
  }
