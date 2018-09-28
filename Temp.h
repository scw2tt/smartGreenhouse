#define Serial SerialUSB

#ifndef TEMP_H
#define  TEMP_H

class Temp {

public:
  // constructor that takes in the pin number
  Temp(int p, float H, float C, bool in){
    pin = p;
    tooCool = C;
    tooHot = H;
    inside = in;
  }


  // checks the temperature using the temperature sensor
  bool checkTooCool(){
   unsigned long V = analogRead(pin);
   if (inside){
    double t = getTempIn();
    
    return (t < tooCool);
   }
   else {
   double t = getTemp();
   return (t < tooCool);
   }
   return false; 
  }

  // checks if the temperature is too warm
  bool checkTooHot(){
   if (inside){
   double t = getTempIn();
   
   return (t > tooHot);
   }
   else{
    double t = getTemp();
    return (t > tooHot);
   }

    return false;
  }
  
  // gets the temperature for the display
  double getTemp(){
    unsigned long V = analogRead(pin);
    //Serial.print("adc: ");
    //Serial.println(V);    
    double t;
    t = (V*(3.3/1024.0)-.5)/(.01);     
    //t = 100*(V*3.3/1024.0-0.5);
    return t;
  }

  double getTempIn(){
    double t;
    unsigned long V = analogRead(pin);
    //Serial.print("adc: ");
    //Serial.println(V);
    //t = 100*(V*3.3/1024.0-0.5);
    t = (V*(3.3/1024.0)-.5)/(.01);
    return t;
  }

  // change temp threshold
  void setTemp(float tH, float tC){
    tooHot = tH;
    tooCool = tC;
  }
  

private:
  int pin;
  float tooHot;
  float tooCool;
  bool inside;
  
};

#endif
