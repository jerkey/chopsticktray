#define BUTTON_TIME     1000    // milliseconds to hold button down before activation happens
#define BUTTON_LOCKOUT  2000    // milliseconds to disable button after activation to prevent accidental reactivation

#define BUTTON_PIN      3
#define LED_PIN         13
#define MOTOR_PIN       10
#define MOTOR_OFF_PIN 12 // when HIGH servo is disabled
#define KNOB_POSE_1_PIN      A1
#define KNOB_POSE_2_PIN      A2
#define KNOB_SPEED_PIN       A3

#include <Servo.h>
Servo motor;  // create servo object to control a servo

#define NUMBER_OF_MODES 2       // how many position modes the button cycles through
uint8_t presentMode = 0;        // which position mode we are presently in
uint16_t lastPosition;          // last position of the motor
uint32_t lastButtonTime = 0;    // when the button last changed state

void setup() {
  Serial.begin(9600);
  Serial.println("github.com/jerkey/chopsticktray");
  pinMode(MOTOR_OFF_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(MOTOR_OFF_PIN, HIGH);    // shut off power to motor
  digitalWrite(BUTTON_PIN, HIGH);       // pull-up resistor on button
  motor.attach(MOTOR_PIN);              // servo library to control motor
  buttonFunction();                     // put motor in a position to initialize
}

void moveMotor(int angle) {
  motor.write(lastPosition);            // start where you are
  digitalWrite(MOTOR_OFF_PIN,LOW);      // enable motor
  int step = 1; // must be 1 for "i != angle" to work
  if (angle < lastPosition) step *= -1; // count the right direction
  for(int i=lastPosition; i != angle;  i += step){
    motor.write(i);
    delay(analogRead(KNOB_SPEED_PIN) / 5);
  }
  motor.write(angle);   // go all the way to final position
  delay(1000); // wait for servo to arrive before disabling it
  digitalWrite(MOTOR_OFF_PIN,HIGH);
  lastPosition = angle;
}

void buttonFunction() {
  presentMode += 1;
  if (presentMode == NUMBER_OF_MODES) presentMode = 0; // cycle back to 0

  if (presentMode == 0) {       // position 0 = knob 1
    moveMotor(analogRead(KNOB_POSE_1_PIN) / 5);   // range of 0-1023 / 5 = 0-204 degrees
  }
  if (presentMode == 1) {       // position 1 = knob 2
    moveMotor(analogRead(KNOB_POSE_2_PIN) / 5);   // range of 0-1023 / 5 = 0-204 degrees
  }
}

void loop() {
  if (digitalRead(BUTTON_PIN) == 0) {   // button is pressed

    if (millis() - lastButtonTime < BUTTON_LOCKOUT) {   // button was pressed too soon after last activation or lockout
      Serial.println("BUTTON_LOCKOUT");
      while (digitalRead(BUTTON_PIN) == 0) { // wait while button stays pressed
        digitalWrite(LED_PIN, (millis() % 1000) < 250);  // blink LED sparsely
      }
      digitalWrite(LED_PIN, LOW);  // turn LED off
      lastButtonTime = millis();    // store last button time


    } else {    // button was pressed down, let's see for how long

      lastButtonTime = millis();        // start button timer
      while (digitalRead(BUTTON_PIN) == 0) { // wait while button stays pressed
        digitalWrite(LED_PIN, (millis() % 500) < 250);  // blink LED rapidly
      }
      digitalWrite(LED_PIN, LOW);  // turn LED off


      if (millis() - lastButtonTime > BUTTON_TIME) {    // button was pressed for long enough to be valid
        Serial.println("BUTTON_TIME");
        digitalWrite(LED_PIN, HIGH);  // turn ON LED
        buttonFunction();             // do the button thing
        digitalWrite(LED_PIN, LOW);   // turn LED off
        lastButtonTime = millis();    // store last button time


      } else {                                          // button was not pressed for long enough to be valid
        Serial.println("BUTTON_SHORT");
        lastButtonTime = millis();    // store last button time
      }
    }
  }
}

