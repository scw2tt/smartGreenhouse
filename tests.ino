  /*
  String d = "10.00, 12.00";
  stringToDouble(d);
  while(true){
    // do nothing
  }
  */
  /*
  // Temperature test: check the temperature and print out the result
  Serial.println(outside->getTemp());
  Serial.println(inside->getTemp());
  delay(5000);
  */
  /*
  // Spoke test
  
  //spoke->rotateF();
  s.attach(9);
  s.write(180);
  delay(400);
  s.detach();
  Serial.println("180");
  delay(3000);
  
  //spoke->rotateB();
  s.attach(9);
  s.write(0);
  delay(400);
  s.detach();
  delay(3000);
  */

  // Heater test
  /*
  Serial.println("testing");
  heat->switchOn();
  delay(80000);
  heat->switchOff();
  delay(40000);
  */

  /*
  // LCD Test
  // get the temperature
  Serial.println("test");
  houseDisplay();
  delay(5000);
  houseSetDisplay();
  delay(5000);
  
  // buttons test
  */
  /*
  {
  if(upButton->checkPressed()){
    Serial.println("up Pressed");
  }

  if (downButton->checkPressed()){
    Serial.println("down pressed");
  }
  }
  */
  //Send Data test
  /*
  SendReadingToDB(6, 34);
  SendReadingToDB(1, 34);
  SendReadingToDB(2, 34);
  SendReadingToDB(3, 34);
  SendReadingToDB(4, 34);
  SendReadingToDB(5, 34);

  delay(5000);
  */


//check the buttons; not needed for the samd configuration
/*
  if (upButton->checkPressed()){
      Serial.println("raise thresholds");
      tooHot += 1;
      tooCold += 1;
      inside->setTemp(tooHot, tooCold);
      outside -> setTemp(tooHot, tooCold);
      houseSetDisplay();
  }

  if (downButton->checkPressed()){
      Serial.println("lower thresholds");
      tooHot -= 1;
      tooCold -= 1;
      inside->setTemp(tooHot, tooCold);
      outside -> setTemp(tooHot, tooCold);  
      houseSetDisplay();
      
  }
*/


  // Spoke test
  
  /*
  s.attach(9);
  s.write(180);
  delay(400);
  s.detach();
  Serial.println("180");
  delay(3000);
  
  
  s.attach(9);
  s.write(0);
  delay(400);
  s.detach();
  delay(3000);
  */
