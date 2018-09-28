#define Serial SerialUSB

#ifndef HEATER_H
#define  HEATER_H

class Heater {

public:
  // constructor that takes in the pin number
  Heater(int p){
    pin = p;
    state = 0.0;
  }


  // turns on the heat from the power resistor
  void switchOn(){
   digitalWrite(8, HIGH);
   Serial.println("heat on!"); 
   state = 3.0;
  }


  // turns off the heat from the power resistor
  void switchOff(){
   digitalWrite(8, LOW); 
   Serial.println("heat off!");
   state = 0.0;
  }

  double getState(){
    return state; 
  }

private:
  int pin;
  double state;

  
};

#endif
