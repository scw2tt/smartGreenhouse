#define Serial SerialUSB

#ifndef TIMER_H
#define  TIMER_H

class Timer {

public:
  
  void start(unsigned long x) {
    startTime = millis();
    //Serial.print(startTime);
    duration = x;
  }

  bool checkExpired() {
    /*Serial.print("Millis(): ");
    Serial.print(millis());
    Serial.print("\n");
    Serial.print("starttime: ");
    Serial.print(startTime);
    Serial.print("\n");
    Serial.print(duration);
    Serial.print("\n");
    Serial.print("-----------------------");
    //delay(100);*/
    
    if(duration < (millis() - startTime)){
      return true;
    }
    return false;
  }
  

private:
  unsigned long startTime;
  unsigned long duration;

};

#endif
