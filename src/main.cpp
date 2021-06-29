#include <Arduino.h>
#include ".\ball_detector.h"

const int BUTTON_PIN = 7;
const int LED_PIN = 8;
const int TWO_POINTS_SENSOR_PIN_1 = A0;
const int TWO_POINTS_SENSOR_PIN_2 = A5;
const int THREE_POINTS_SENSOR_PIN = A1;
const int ONE_POINT_SENSOR_PIN = A4;

Ball_detector two_points_sensor1(TWO_POINTS_SENSOR_PIN_1);
Ball_detector two_points_sensor2(TWO_POINTS_SENSOR_PIN_2);
Ball_detector three_points_sensor(THREE_POINTS_SENSOR_PIN);
Ball_detector one_point_sensor(ONE_POINT_SENSOR_PIN);

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

void setup()
{

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  static int count1 = 0;
  static int count2 = 0;
  static int count3 = 0;
  static int score = 0;
  static bool ball_found;
  static const char INNER_GOAL_C = 'I';
  static const char OUTER_GOAL_C = 'O';
  static const char LOWER_GOAL_C = 'L'; //change later

  int buttonVal = digitalRead(BUTTON_PIN);

  if (LEDLongPulse(LED_PIN, ball_found))
  { //enters if long press finished
    blinkLED(LED_PIN, 300, 600);
  }

  ball_found = false;

  if (one_point_sensor.ball_detected())
  {
    count1++;
    ball_found = true;

    Serial.print(LOWER_GOAL_C);
  }

  if (two_points_sensor1.ball_detected())
  {
    count2++;
    ball_found = true;

    Serial.print(OUTER_GOAL_C);
  }

  if (two_points_sensor2.ball_detected())
  {
    count2++;
    ball_found = true;
    Serial.print(OUTER_GOAL_C);
  }

  if (three_points_sensor.ball_detected())
  {
    count3++;
    ball_found = true;

    Serial.print(INNER_GOAL_C);
  }

  if (longPress(1000, !buttonVal))
  {
    count2 = 0;
    count3 = 0;
    score = 0;

    LEDLongPulse(LED_PIN, true);
  }
  Serial.print(one_point_sensor.get_value());
  Serial.print("\t");
  Serial.print(two_points_sensor1.get_value());
  Serial.print("\t");
  Serial.print(two_points_sensor2.get_value());
  Serial.print("\t");
  Serial.println(three_points_sensor.get_value());
}