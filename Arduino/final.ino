
#include "MeShield.h"

MeStepper stepper1(1), stepper2(2), stepper3(9);
int position1, position2, position3;

bool stringComplete = false;
String inputString = "";
int maxCount = 5000;
int moveSpeed = 8000;

void myRun1(MeStepper *myStepper, int position) {
  myStepper->moveTo(position);
  myStepper->setMaxSpeed(moveSpeed);
  int m = maxCount, n = 0;
  while (m > 0 && myStepper->distanceToGo() != 0) {
  	myStepper->runSpeedToPosition();
    n++;
    if (n >= 50) {
      delay(1);
      m--;
      n = 0;
    }
  }
}

void myRun2(MeStepper *myStepper1, int position1, MeStepper *myStepper2, int position2) {
  myStepper1->moveTo(position1);
  myStepper1->setMaxSpeed(moveSpeed);
  myStepper2->moveTo(position2);
  myStepper2->setMaxSpeed(moveSpeed);
  int m = maxCount, n = 0;
  while (m > 0 && (myStepper1->distanceToGo() != 0 || myStepper2->distanceToGo() != 0)) {
  	myStepper1->runSpeedToPosition();
  	myStepper2->runSpeedToPosition();
    n++;
    if (n >= 50) {
      delay(1);
      m--;
      n = 0;
    }
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == 'z')
    	stringComplete = true;
    else
      inputString += inChar;
  }
}

void setup() {
	Serial.begin(9600);
}

void loop() {
	delay(100);
	serialEvent();
	if (stringComplete) {
		sscanf(inputString.c_str(), "%d %d %d", &position1, &position2, &position3);
		
		//Serial.println("wait");
		myRun2(&stepper1, position1, &stepper2, position2);
		myRun1(&stepper3, position3);

		Serial.println("done");
		
		inputString = "";
		stringComplete = false;
	}
}
