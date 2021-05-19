#define REMOTEXY_MODE__SOFTSERIAL
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <ArduinoJson.h>

const byte BUTTON_PIN = 7;
const byte LED_PIN = 8;  
const byte TWO_POINTS_SENSOR_PIN = A0;
const byte THREE_POINTS_SENSOR_PIN = A1;

SoftwareSerial bluetooth(2, 3);

bool longPress(const unsigned int wantedTime, const bool buttonVal)
{
  // return true if button pressed for wanted amount of time
  static unsigned long pressedTime = 0; //the milli when the button was pressed
  static bool lastBVal = false;         //for rising edge
  static bool hasSucceeded = false;     //true if succeeded false if didnt succeed or finish
  unsigned long mil = millis();
  bool hasPressedLongEnough = mil - pressedTime >= wantedTime;

  if (buttonVal && !lastBVal)
  {
    pressedTime = mil;
    hasSucceeded = false;
  }
  if (buttonVal)
  {
    if (hasPressedLongEnough && !hasSucceeded)
    {
      hasSucceeded = true;
      Serial.println("Count reset! ;)");
      // return true;
    }
  }
  else
  {
    hasSucceeded = false;
  }

  lastBVal = buttonVal;
  return hasPressedLongEnough && hasSucceeded;
}

bool LEDLongPulse(int pinNum, bool state)
{
  //blinks once a LED connected to digital output pin - pinNum.
  //if state == true:
  // 1) LED is turned on.
  // 2) saving current time.
  //if state == false:
  // 1) checking pulse time expired
  // 2) if so, turning the LED off.
  // return value: task completed indication.

  static unsigned long onTime;
  static unsigned int pulseTime = 1000;
  static bool isDone;
  unsigned long mil = millis();

  pinMode(pinNum, OUTPUT);

  if (state)
  {
    digitalWrite(pinNum, HIGH);
    onTime = mil;
    isDone = false;
  }
  else
  {
    if (mil - onTime >= pulseTime)
    {
      digitalWrite(pinNum, LOW);
      isDone = true;
    }
    else
    {
      isDone = false;
    }
  }
  return isDone;
}

void blinkLED(int pinNum, unsigned int onTime, unsigned int offTime)
{
  //blinks a LED connected to digital output pin - pinNum.
  // onTime and offTime are in milliseconds.
  static unsigned long changeTime;
  static bool ledState;
  unsigned long mil = millis();

  pinMode(pinNum, OUTPUT);
  //  turn the LED on
  if (!ledState && mil - changeTime >= offTime)
  {
    digitalWrite(pinNum, HIGH);
    changeTime = mil;
    ledState = !ledState;
  }
  //  turn the LED off
  if (ledState && mil - changeTime >= onTime)
  {
    digitalWrite(pinNum, LOW);
    changeTime = mil;
    ledState = !ledState;
  }
}
bool ball_detected(int sensor_val)
{
  static const int THRESHOLD_HIGH = 600;
  static const int THRESHOLD_LOW = 400;
  static bool curr_state; //true=sensing ball false=not sensing ball
  static bool last_state;

  if (sensor_val > THRESHOLD_HIGH)
  {
    curr_state = true;
  }
  else if (sensor_val < THRESHOLD_LOW)
  {
    curr_state = false;
  }

  if (last_state && !curr_state)
  {
    last_state = curr_state;
    return true;
  }

  last_state = curr_state;
  return false;
}

void setup()
{

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  bluetooth.begin(9600);
}

void loop()
{

  static bool lastState0;
  static bool lastState1;
  static bool currentState0; //true=sensing ball false=not sensing ball
  static bool currentState1; //true=sensing ball false=not sensing ball
  static int count0 = 0;
  static int count1 = 0;
  static int score = 0;
  static const int THRESHOLD_HIGH = 600;
  static const int THRESHOLD_LOW = 400;
  static bool isBall;
  unsigned long mil = millis();
  static unsigned long reset_time = 0;
  static const char INNER_GOAL_C = 'A'; //change later 
  static const char OUTER_GOAL_C = 'A'; //change later 

  int sensorValue0 = analogRead(TWO_POINTS_SENSOR_PIN);
  int sensorValue1 = analogRead(THREE_POINTS_SENSOR_PIN);
  int buttonVal = digitalRead(BUTTON_PIN);

  if (LEDLongPulse(LED_PIN, isBall))
  { //enters if long press finished
    blinkLED(LED_PIN, 300, 600);
  }

  if (sensorValue0 > THRESHOLD_HIGH)
  {
    currentState0 = true;
  }
  else if (sensorValue0 < THRESHOLD_LOW)
  {
    currentState0 = false;
  }

  if (sensorValue1 > THRESHOLD_HIGH)
  {
    currentState1 = true;
  }
  else if (sensorValue1 < THRESHOLD_LOW)
  {
    currentState1 = false;
  }

  isBall = false;

  if (lastState0 && !currentState0)
  {
    count0++;
    isBall = true;

    Serial.print(INNER_GOAL_C);

    score += ((mil - reset_time) <= 20000 ? 4 : 2);
  }

  if (lastState1 && !currentState1)
  {
    count1++;
    isBall = true;
    
    Serial.print(OUTER_GOAL_C);

    score += ((mil - reset_time <= 20000) ? 6 : 3);
  }

  if (longPress(1000, !buttonVal))
  {
    count0 = 0;
    count1 = 0;
    score = 0;

    LEDLongPulse(LED_PIN, true);

    reset_time = mil;
  }

  lastState0 = currentState0;
  lastState1 = currentState1;
}