#define Serial SerialUSB

#ifndef SPOKE_H
#define  SPOKE_H

#include <Servo.h>

class Spoke {

public:
  // constructor that takes in the pin number
  Spoke(Servo m, int p){
    pin = p;
    motor = m;
    motor.attach(A4);
    motor.write(0);
    delay(500);
    motor.detach();
    Serial.println("spoke created");
    
  }


  // lifts the rod up
  void rotateF(){
    motor.attach(A4);
    motor.write(180);
    delay(500);
    motor.detach();
    Serial.println("rotated f");
    state = "up";
    return;
  }

  void rotateB(){
    motor.attach(A4);
    motor.write(0);
    delay(500);
    motor.detach();
    Serial.println("rotated b");
    state = "down";
    return;
  }
  
  String getState(){
    return state;
  }
  

private:
 
  Servo motor;
  int pin;
  String state;
  
};

#endif


 
