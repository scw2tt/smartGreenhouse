#include "Temp.h"
#include "Heater.h"
#include "timer.h"
#include "button.h"
#include "model.h"
#include <Servo.h>

/////////////////////
// Software Serial, if you're that sort... //
/////////////////////
#ifdef USE_SOFTSERIAL
#include <SoftwareSerial.h>
#include <ESP8266_AT.h>
#include <ESP8266_defines.h>
#include <ESP8266_thin.h>
#define ESP8266_SW_RX    9    // ESP8266 UART0 RXI goes to Arduino pin 9
#define ESP8266_SW_TX    8    // ESP8266 UART0 TXO goes to Arduino pin 8
static SoftwareSerial swSerial(ESP8266_SW_TX, ESP8266_SW_RX);
#endif

#define Serial SerialUSB

//////////////////////
// Library Includes //
//////////////////////
// SoftwareSerial is required (even you don't intend on
// using it).
#include <ESP8266_thin.h>

//////////////////////////////
// WiFi Network Definitions //
//////////////////////////////
// Replace these two character strings with the name and
// password of your WiFi network.
const char mySSID[] = "wahoo";
const char myPSK[] = "";

const char destServer[] = "ec2-34-209-142-24.us-west-2.compute.amazonaws.com";

// uploads the data to the database that will be displayed on the webpage
const char insertDB[] = "GET /~scw2tt/insertDB.php";   //just call the basic webpage

// receives the signal from the internet to update the setpoints
const char getCurrent[] = "GET /~scw2tt/getCurrent.php";   //grabs that bottom value


// Pins
const int tempPinIn = A0; //A2 for arduino
const int tempPinOut = A3;
const int heatPin = 8;

//buttons not needed for samd
//const int upPin = 4;   
//const int downPin = 5;

// Thresholds
double tooHot;
double tooCold;


// Start the Current State at 1
int currentState = 1;

// Start the heater state
double lState = 0;

//------------------------------------------------------------------------
// Objects
Servo s;
unsigned int SERVO_DELAY = 600;
//SoftwareSerial mySerial(3,2);
Temp * inside = new Temp(tempPinIn, tooHot, tooCold, true);
Temp * outside = new Temp(tempPinOut, tooHot, tooCold, false);
Heater * heat = new Heater(heatPin);
double heaterState = 0.0; 
Timer timer;
Timer timer30;
char inString[5] , outString[5],tC[5], tH[5]; // create string arrays

//Button * upButton = new Button(upPin);
//Button * downButton = new Button(downPin);

//------------------------------------------------------------------------
// Serial Communication stuff
#include <wiring_private.h> //needed for the pinPeripheral() function

//establishes a new SERCOM on pins 3 (RX) and 4 (TX) -- though we won't actually use 3
Uart mySerial (&sercom2, 3, 4, SERCOM_RX_PAD_1, UART_TX_PAD_0);

//sets up the IRQ handler
void SERCOM2_Handler()
{
  mySerial.IrqHandler();
}

const int led = 13;


// used for the model
double inTemp;
const double KH = 0.7;
const double CH = 180.0;
const double K = 6.0;
const double C= 120.0;
const double DT= 5.0;
double Q = 0.0;
double inSim;
double preSim;

// for the heater simulation
double heatSim0 = 0;
double heatSim;
double heatSim1;
const double startTemp = 24.0;

const double THRESHOLD_DIFF = 4.0;
const double WINDOW_SIZE = 60; // 5 minutes
const double ALLOW_PCT = 0.25; // be concerned if 75% of data is above the threshold

model Simulation = model(THRESHOLD_DIFF, WINDOW_SIZE, ALLOW_PCT);
bool alert = false;

//-----------------------------------------------------------------------
//setup code
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500); // wait for the display to boot up
  Serial1.begin(115200);
  mySerial.begin(9600);
  pinPeripheral(4, PIO_SERCOM_ALT);
  // initializeESP8266 verifies communication with the WiFi
  // shield, and sets it up.
  initializeESP8266();

  // connectESP8266 connects to the defined WiFi network.
  connectESP8266();

  // displayConnectInfo prints the Shield's local IP
  // and the network it's connected to.
  displayConnectInfo();
  
  pinMode(tempPinIn, INPUT); // for the temperature sensor
  pinMode(tempPinOut, INPUT); // for the outside temperature sensor
  
  pinMode(8, OUTPUT); // for the heater
  pinMode(2, OUTPUT); // display
  //pinMode(5, INPUT_PULLUP);  // buttons
  //pinMode(4, INPUT_PULLUP);

  pinMode(9, OUTPUT);
  s.attach(9);
  //Serial.print("attached");
  s.write(0);
  delay(100);
  s.detach();
  tooHot = 26.0;
  tooCold = 23.0;
  
  //ffor demo
  //heat.switchOn();
  
    //top row
  mySerial.write(254);
  mySerial.write(128);
  mySerial.write("              ");
  mySerial.write("              "); 

  // bottom row
  mySerial.write(254);
  mySerial.write(192);
  mySerial.write("              ");
  mySerial.write("              ");
  

  //declare the appropriate mode for the serial pin
  //begins the UART
  

  //this line declares the pin functionality
  //it must go after the begin() call, for some reason
  //note that pin 3 will have normal functionality
  // initialize the software timers
  timer.start(5000);
  timer30.start(30000);

  // begin the simulation
  preSim = inside->getTempIn();
  heatSim = preSim;

}

void loop() {
  

  


  
  // put your main code here, to run repeatedly:  
  // main loop through the state machine
  // -----------state machine code------------------------
  // switch case for the code
  /*
  * Case 1: Inactive 
  * Case 2: Heating
  * Case 3: Cooling
  */
  
  int nextState;
 
  if (timer.checkExpired()){
  // update the temperature
  inTemp = inside ->getTempIn();

  
  switch(currentState){
    
    case 1:   // Inactive
        //Serial.println("temp in");
        //Serial.println(inTemp);
        houseDisplay(heat->getState());  
        // if it is too cold, then turn the heat on
        if (inTemp < tooCold){
          Serial.println("too cold");
          heat->switchOn();
          heaterState = 3.0;
          nextState = 2;
          Q = 55.0;
          break;
          }
          
         // if it is too hot, then open the roof

         else if (inTemp > tooHot){
          Serial.println("too hot");
          s.attach(9);
          s.write(180);
          delay(SERVO_DELAY);
          s.detach();
          nextState = 3;
          lState = 3.0;
          break;
         }
         
         // this means that nothing is happening 
         else{
          Serial.println("nothing");
          nextState = 1;
          break;
         }
      
      // if the timer isn't expired, then stay in the same state
      
      break;
     
    case 2: // heating
        houseDisplay(heat->getState());
        // if it is too hot, then turn off the heat 
        if (inTemp> tooCold + 1.0){
          heat->switchOff();
          nextState = 1;
          Q = 0.0;
          break;
        }

        if (inTemp > tooHot){
          heat->switchOff();
          nextState = 3;
          Q = 0.0;
        }
       
        else {
        nextState = 2;
        }
        break;
       
    
    case 3: // cooling
    // if it is too cold, then close the roof
        houseDisplay(heat->getState());
        if (inTemp< tooHot){
          s.attach(9);
          s.write(0);
          delay(SERVO_DELAY);
          s.detach();
          nextState = 1;
          lState = 0.0;
          break;
        }
        
        else{
        nextState = 3;
        }
      break;
     
  }

  // restart the timer
  houseDisplay(heat->getState());
  currentState = nextState;
  Serial.print("NEXT STATE -------------------");
  Serial.println(nextState);
  
  timer.start(5000);
  }


  

  // update the database and show the set points
  if (timer30.checkExpired()){
  houseSetDisplay();
  Serial.println("30 Seconds");
  double Tin = inTemp;
  double Tout = outside->getTemp();
  double hState = heat->getState();

  // put the difference from the projection in the model
  simulateHeat(); // updates the heatSim using the preSim
  simulate(); // updates the inSim using the preSim
  heatSim = heatSim1; // set the next projected value to be use
  preSim = inSim; // set the next preSim 
  if (inSim != NULL){
    Simulation.insert(inSim - inTemp);
    alert = Simulation.getAlert(); // gets the alert variable to see if it is needed
    if (alert){
      Serial.println("-----------------------------");
      Serial.println(" SYSTEM HAS BEEN COMPROMISED ");
      Serial.println("-----------------------------");

    }
  }
  
  SendReadingToDB(1, Tin);
  SendReadingToDB(2, Tout);
  SendReadingToDB(3, hState);
  SendReadingToDB(4, tooHot);
  SendReadingToDB(5, tooCold);
  SendReadingToDB(6, lState);
  
  SendReadingToDB(7, inSim); // not actual sensor, for the model
  if (alert){
    SendReadingToDB(8, 1.0); // not actual sensor, for the model
  } else{
    SendReadingToDB(8, 0.0);
  }
  // get the setpoints from the website
  getSetpoints();
  

  timer30.start(30000);
  }
  
  
  
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

void houseDisplay(double hState){
  if (hState < 1){
  clearDisplay();
  double Tin = inTemp;
  double Tout = outside->getTemp();
  Tout = Tout;
  Tin = Tin;
  //Serial.println(Tin);
  
  //sprintf(inString, "%4d", Tin);
  //sprintf(outString, "%4d", Tout);
  //sprintf(tC, "%f", tooCold);
  //sprintf(tH, "%f", tooHot); 

  String TI = String(Tin);
  String TO = String(Tout);
  TI.toCharArray(inString, 5);
  TO.toCharArray(outString, 5);
  
  //top row
  mySerial.write(254);
  mySerial.write(128);
  mySerial.write("In:  ");
  mySerial.write(inString);
  //mySerial.write("Too Hot: ");
  //mySerial.write(tH); 

  // bottom row
  mySerial.write(254);
  mySerial.write(192);
  mySerial.write("Out: ");
  mySerial.write(outString);
  //mySerial.write("Too Cold: ");
  //mySerial.write(tC);
  } 
  else{
  clearDisplay();
  double Tin = inTemp;
  double Tout = outside->getTemp();
  Tout= Tout;
  Tin = Tin;
  //Serial.println(Tin);
  
  //sprintf(inString, "%4d", Tin);
  //sprintf(outString, "%4d", Tout);
  //sprintf(tC, "%f", tooCold);
  //sprintf(tH, "%f", tooHot); 

  String TI = String(Tin);
  String TO = String(Tout);
  TI.toCharArray(inString, 5);
  TO.toCharArray(outString, 5);
  
  //top row
  mySerial.write(254);
  mySerial.write(128);
  mySerial.write("*In:  ");
  mySerial.write(inString);

  // bottom row
  mySerial.write(254);
  mySerial.write(192);
  mySerial.write("Out: ");
  mySerial.write(outString);
  //mySerial.write("Too Cold: ");
  //mySerial.write(tC);  



    
  }
}

void houseSetDisplay(){
  //Serial.println(Tin);
  clearDisplay();
  
  String TH = String(tooHot);
  String TC = String(tooCold);
  TH.toCharArray(tH, 5);
  TC.toCharArray(tC, 5);

  //top row
  
  mySerial.write(254);
  mySerial.write(128);
  mySerial.write("ColdSP:  ");
  mySerial.write(tH); 

  // bottom row
  mySerial.write(254);
  mySerial.write(192);
  mySerial.write("HotSP: ");
  mySerial.write(tC);
  
}

void clearDisplay(){
  //top row
  
  mySerial.write(254);
  mySerial.write(128);
  mySerial.write("              ");
  mySerial.write("              "); 

  // bottom row
  mySerial.write(254);
  mySerial.write(192);
  mySerial.write("              ");
  mySerial.write("              ");
  
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// function for inserting to table over the cloud
void SendReadingToDB(int id, double value){
  // To use the ESP8266 as a TCP client, use tcpConnect()
  
  // tcpConnect([server], [port]) is used to 
  // connect to a server (const char * or IPAddress) on
  // a specified port.
  // Returns: 1 on success, 2 on already connected,
  // negative on fail (-1=TIMEOUT, -3=FAIL).
  int retVal = esp8266.tcpConnect(destServer, 80);
  if (retVal <= 0)
  {
    Serial.print(retVal);
    Serial.println(F(" -- Failed to connect to server."));
    return;
  }

  Serial.print("Checking connection: "); //just to verify
  Serial.println(esp8266.connected());

  // the next lines show the general workflow: begin, send, end
  esp8266.tcpBeginTransmission();   //library call to start transmission
  esp8266.tcpSendPacket(insertDB); //the web page we want

  // interiorTemp, exteriorTemp, spheat, spcool, heater, lidPosition
  //send the data fields:
 
  esp8266.tcpSendPacket(String("?sensor_id=") + String(id));
  esp8266.tcpSendPacket(String("&value=") + String(value));
  

  
  //etc., etc.

  
  esp8266.tcpSendPacket(" HTTP/1.1\r\nHost: "); //the beginning of the "footer"
  esp8266.tcpSendPacket(destServer); //this tells the interwebs to route your request through the AWS server
  esp8266.tcpSendPacket("\r\nConnection: close\r\n\r\n");  //tells the server to close the cxn when done. \r\n\r\n tells it we're done
  esp8266.tcpEndTransmission();     //library call to close transmission on our end

  Serial.println("Request sent.");

  int16_t rec = 1;

  //check for a response. rec holds the number of bytes in each packet: negative numbers are errors
  //when the server is done, tcpReceive will timeout and return 0 (no bytes), in which case, we're done
  while(rec > 0)
  {
    char recBuffer[512];
    rec = esp8266.tcpReceive(recBuffer, 512); //returns the number of bytes received; recBuffer holds a line

    //print the number of characters received (just for debugging -- comment out when using for realsies)
    Serial.print(rec);
    Serial.print(':');

    if(rec > 0) //if we got data, print it to the Serial Monitor
    {
      Serial.print(recBuffer); 
    }
  }

  Serial.println("Done."); 

  // connected() is a boolean return value: 1 if the 
  // connection is active, 0 if it's closed.
  if (esp8266.connected())
  {
    Serial.println("Closing.");  
    esp8266.close(); //explicitly close the TCP connection.
  }
}





//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// get the set points
void getSetpoints(){

  // To use the ESP8266 as a TCP client, use tcpConnect()
  
  // tcpConnect([server], [port]) is used to 
  // connect to a server (const char * or IPAddress) on
  // a specified port.
  // Returns: 1 on success, 2 on already connected,
  // negative on fail (-1=TIMEOUT, -3=FAIL).
  int retVal = esp8266.tcpConnect(destServer, 80);
  if (retVal <= 0)
  {
    Serial.print(retVal);
    Serial.println(F(" -- Failed to connect to server."));
    return;
  }

  Serial.print("Checking connection: "); //just to verify
  Serial.println(esp8266.connected());

  // the next lines show the general workflow: begin, send, end
  esp8266.tcpBeginTransmission();   //library call to start transmission
  esp8266.tcpSendPacket(getCurrent); //the web page we want
  
  //etc., etc.

  
  esp8266.tcpSendPacket(" HTTP/1.1\r\nHost: "); //the beginning of the "footer"
  esp8266.tcpSendPacket(destServer); //this tells the interwebs to route your request through the AWS server
  esp8266.tcpSendPacket("\r\nConnection: close\r\n\r\n");  //tells the server to close the cxn when done. \r\n\r\n tells it we're done
  esp8266.tcpEndTransmission();     //library call to close transmission on our end

  Serial.println("Request sent.");

  // read in the double 
  int16_t rec = 1;
  double out;
  String data;
  //check for a response. rec holds the number of bytes in each packet: negative numbers are errors
  //when the server is done, tcpReceive will timeout and return 0 (no bytes), in which case, we're done
  while(rec > 0)
  {
    char recBuffer[512];
    rec = esp8266.tcpReceive(recBuffer, 512); //returns the number of bytes received; recBuffer holds a line

    //print the number of characters received (just for debugging -- comment out when using for realsies)
    Serial.print(rec);
    Serial.print(':');

    if(rec > 0) //if we got data, print it to the Serial Monitor
    {
      Serial.print(recBuffer);
      String d(recBuffer); 
      data = d;
      int start = data.indexOf(">");
      int e = data.lastIndexOf("<");
      data = data.substring(start, e);
      //a , b
      int coolEnd = data.indexOf(",");
      String tCold = data.substring(1, coolEnd);
      String tHot = data.substring(coolEnd+2); 
      
      double cCheck = tCold.toDouble();
      double hCheck = tHot.toDouble();
      
      if (cCheck != 0.0 && hCheck != 0.0){
      tooCold = tCold.toDouble();
      tooHot = tHot.toDouble();
      break;
      }  
      
      Serial.println("Too Cold: ");
      Serial.println(tooCold);

      
      Serial.println("Too Hot: ");
      Serial.println(tooHot);

      
      Serial.println("data: ");
      Serial.print(data);

      // set the output
    }
    
  }

  Serial.println("Done."); 

  // connected() is a boolean return value: 1 if the 
  // connection is active, 0 if it's closed.
  if (esp8266.connected())
  {
    Serial.println("Closing.");  
    esp8266.close(); //explicitly close the TCP connection.
  }
  return;
}


//---------------------------------------------
// parse the string and get the setpoints
void stringToDouble(String s){
  int mid = s.indexOf(',');
  String cool = s.substring(0, mid - 1);
  String hot = s.substring(mid + 2, s.length() -1);

  double c = cool.toDouble();
  double h = hot.toDouble();

  Serial.println(c);
  Serial.println(h);
  
}


//--------------------------------------------------------------------------------------
void initializeESP8266()
{
  // esp8266.begin() verifies that the ESP8266 is operational
  // and sets it up for the rest of the sketch.
  // It returns either true or false -- indicating whether
  // communication was successul or not.
  // true
  int test = esp8266.begin(&Serial1);
  if (test != true)
  {
    Serial.println(F("Error talking to ESP8266."));
    errorLoop(test);
  }
  Serial.println(F("ESP8266 Shield Present"));
}
//-----------------------------------------------------------------------------------------
void connectESP8266()
{
  // The ESP8266 can be set to one of three modes:
  //  1 - ESP8266_MODE_STA - Station only
  //  2 - ESP8266_MODE_AP - Access point only
  //  3 - ESP8266_MODE_STAAP - Station/AP combo
  // Use esp8266.getMode() to check which mode it's in:
  int retVal = esp8266.getMode();
  if (retVal != ESP8266_MODE_STA)
  { // If it's not in station mode.
    // Use esp8266.setMode([mode]) to set it to a specified
    // mode.
    retVal = esp8266.setMode(ESP8266_MODE_STA);
    if (retVal < 0)
    {
      Serial.println(F("Error setting mode."));
      errorLoop(retVal);
    }
  }
  Serial.println(F("Mode set to station"));

  char macAddr[24]; //???
  esp8266.localMAC(macAddr);
  Serial.println(macAddr);

  // esp8266.status() indicates the ESP8266's WiFi connect
  // status.
  // A return value of 1 indicates the device is already
  // connected. 0 indicates disconnected. (Negative values
  // equate to communication errors.)
  retVal = esp8266.status();
  
  if (retVal == 5) //ick
  {
    Serial.print(F("Connecting to "));
    Serial.println(mySSID);
    // esp8266.connect([ssid], [psk]) connects the ESP8266
    // to a network.
    // On success the connect function returns a value >0
    // On fail, the function will either return:
    //  -1: TIMEOUT - The library has a set 30s timeout
    //  -3: FAIL - Couldn't connect to network.
    retVal = esp8266.connect(mySSID, myPSK);
  }
  
  if (retVal < 0)
  {
    Serial.println(F("Error connecting"));
    errorLoop(retVal);
  }

  retVal = esp8266.status();
}
//------------------------------------------------------------------------------------------------------
void displayConnectInfo()
{
  char connectedSSID[24];
  memset(connectedSSID, 0, 24);
  // esp8266.getAP() can be used to check which AP the
  // ESP8266 is connected to. It returns an error code.
  // The connected AP is returned by reference as a parameter.
  int retVal = esp8266.getAP(connectedSSID);
  if (retVal > 0)
  {
    Serial.print(F("Connected to: "));
    Serial.println(connectedSSID);
  }

  // esp8266.localIP returns an IPAddress variable with the
  // ESP8266's current local IP address.
  IPAddress myIP = esp8266.localIP();
  Serial.print(F("My IP: ")); Serial.println(myIP);
}

void errorLoop(int error)
{
  Serial.print(F("Error: ")); Serial.println(error);
  Serial.println(F("Entering debug mode."));
  while(1)
  {
    while (Serial.available())
    {
      char ch = Serial.read();
      Serial.print(ch);
      Serial1.write(ch);
    }
  
    while (Serial1.available())
    {
      char ch = Serial1.read();
      Serial.write(ch);
    }
  }
}


/*
 * // used for the model
double inTemp;
double KH;
double CH;
double K;
double C;
double DT;
double inSim;
 */
void simulate(){
  // use the previous simulated value to compute this one instead of the actual measurement
  inSim = (DT/C)*(K*(outside->getTemp() - preSim) + KH*(heatSim - preSim)) + preSim;
}

void simulateHeat(){
  // update the heat
  heatSim1 = (DT/CH)*(KH*(preSim - heatSim) + Q) + heatSim;
}



 

