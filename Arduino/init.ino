#include "MeOrion.h"
#include "MeStepper.h"
MeStepper stepper1(9);
void myRun(MeStepper * myStepper, int position) {
  myStepper->moveTo(position);
  myStepper->setMaxSpeed(3000);
  while (myStepper->distanceToGo() != 0) {
   myStepper->runSpeedToPosition();
  }
}

void setup(){
    myRun(&stepper1, 3000);
}
void loop(){
    delay(10000);
}
