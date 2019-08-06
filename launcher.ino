//  Paper Rocker Launcher Controller
//  Norman L. Brewer, December 2017
// constants won't change. They're used here to set pin numbers:
const int armPin = 2;    // the number of the pushbutton pin
const int launchPin = 3;
const int ledPin = 13;      // the number of the LED pin
const int flasherPin = 4;
const int hornPin = 5;  //  horn is on 5.  use 13 for silent testing
const int solenoidPin = 6;
const int powerPin = 7;
const int imaxCount = 7;

// Variables will change:
int iState = 1;
int iCount = 0;
int iledMode = LOW;
int flashspeed = 100;
bool bLaunched = false;
bool bSilent = false;
int iTimer = 0;


void setup() {
  pinMode(armPin, INPUT_PULLUP);  //switch to arm launcher
  pinMode(launchPin, INPUT_PULLUP);  //pushbutton to launch rocket
  pinMode(flasherPin, OUTPUT);  //flasher to show launcher is armed
  pinMode(hornPin, OUTPUT);  //horn to indicate launch pending
  pinMode(solenoidPin, OUTPUT);  //acutal valve signal to launch
  pinMode(ledPin, OUTPUT);
  pinMode(powerPin, OUTPUT);
  iState = 1; //launcher is implemented as a state machine, starting in state 1
}
void loop() {
  switch (iState) {
    case 1:  //waiting for arm key on
      flashspeed = 50;
      digitalWrite(flasherPin, LOW);
      digitalWrite(hornPin, LOW);
      digitalWrite(solenoidPin, LOW);
      bLaunched = false;
      if (!digitalRead(armPin)) { //if armed, go to state 2
        digitalWrite(flasherPin, HIGH);
        iState = 2;
      }
      if (!digitalRead(launchPin)) { //if not armed and launch pin pressed
        iState = 4;
      }
      break;

    case 2:  //waiting for launch button
      flashspeed = 100;
      if (digitalRead(armPin)) {
        digitalWrite(flasherPin, LOW);
        iState = 1;
      } else {
        if (!digitalRead(launchPin)) {
          if (! bSilent)digitalWrite(hornPin, HIGH);
          iState = 3;
          iCount = 0;  //reset count down timer for launch
        }
      }
      break;

    case 3:  //Launch button pressed, counting down
      if (digitalRead(armPin)) { //if no longer armed
        digitalWrite(flasherPin, LOW);
        digitalWrite(hornPin, LOW);
        iState = 1;
      } else {  //still armed, begin countdown
        iCount += 1;
        flashspeed = 800;
      }
      if (iCount >=  imaxCount) {
        if (digitalRead(armPin)) { //if no longer armed, turn off all outputs
          digitalWrite(solenoidPin, LOW);
          digitalWrite(flasherPin, LOW);
          digitalWrite(hornPin, LOW);
          iState = 1;
        } else {  //still armed, launch and turn off horn
          if (! bLaunched) {
            digitalWrite(solenoidPin, HIGH);
            digitalWrite(hornPin, LOW);
            bLaunched = true;
            delay(1200);
            digitalWrite(solenoidPin, LOW);
          } else {
            flashspeed = 200;
          }
        }
      }  //end maxcount reached
      break;

    case 4 :  //Launch button pressed while not in mode 2
      digitalWrite(powerPin, LOW);
      delay(50);
      iTimer = 0;
      while (!digitalRead(launchPin)) {  //while button pressed
        delay(100);
        iTimer += 100;
        if (iTimer > 3000) {
          digitalWrite(powerPin, HIGH);
          while (!digitalRead(launchPin)) {
            delay(100);
            iTimer += 100;
            if ((digitalRead(launchPin)) && iTimer < 4000) {
              bSilent = true;
              digitalWrite(powerPin, LOW);
              iState = 1;
            } else
              iTimer = 0;
          }
        }
      }
      iState = 1;
      break;

  } //end switch

  delay(flashspeed);
  iledMode = ! iledMode;
  digitalWrite(powerPin, iledMode);
} //end loop
