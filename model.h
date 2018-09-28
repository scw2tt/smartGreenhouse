#define Serial SerialUSB

#ifndef MODEL_H
#define MODEL_H

class model{
  private:
    double threshold;
    int currIndex;
    double diffs[10];
    double allowance;
    double negPct;
    bool alert;

  public:
  // constructor that sets up the threshold difference the other stuff
    model(double th, int maxIndex, double allow) {
        //modSize = maxIndex;
        threshold = th;
        currIndex = 0;
        negPct = 0.0;
        allowance = allow;
        
      } 

   void checkFit(){
     // averages all of the differences and returns true if the mean is above the threshold
     
     double sum = 0.0;
     for (int i = 0; i < 10; i ++){
      if (diffs[i] > threshold){
        sum += 1;
      }
     }

     // get the percent that are off
     negPct = sum/ 10.0;
     if (negPct > allowance){
      alert = true;
      return;
     }
     alert = false;
     return;
   }
   
   // checks if the button is pressed
   void insert(double val){
    // only insert absolute value
    if (val < 0.0){
      val = -1.0 * val;
    }
    
    // if the array is already full, clear it and see if the system is on track
    if (currIndex == 10){
      currIndex = 0;
      checkFit();
      return;
    }else{
      diffs[currIndex] = val;
      currIndex ++;
      return;
    }
   }

  bool getAlert(){
    return alert;
  }

  
};
#endif
