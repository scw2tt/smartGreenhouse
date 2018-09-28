#define Serial SerialUSB

#ifndef BUTTON_H
#define BUTTON_H
#include "timer.h"

class Button{
  private:

    int pin;
    
    int prevState = 0;

  public:
  // constructor that sets the button to a certain pin
    Button(int p) {
      pin = p;
      pinMode(p, INPUT_PULLUP);
      } 

   
   // checks if the button is pressed
    bool checkPressed(void){
      
      int curState = digitalRead(pin);

      if (curState != prevState && curState == LOW){
        prevState =0;
        delay(60);
        return true;
      }


      else if (curState != prevState && curState == HIGH){
        prevState =1;
        delay(60);
        return false;
      }     

      else {
        
        return false;
      }
    }
    

  
};
#endif
