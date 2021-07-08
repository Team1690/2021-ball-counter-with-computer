#include <Wire.h>
#include <MPU6050.h>
#include <LowPower.h>

#define MPUPin 5
#define redPin 3
#define greenPin 4
#define bluePin 2

MPU6050 mpu;

float pitch_offset = 0;
float angle;
float prev_angle;
bool Awake;
bool enable_wake = true;
char color;
int wake_counter = 0;
int calib_counter = 0;
double currentTime = 0;
double wakeTime = 0;
Vector normAccel;
Vector temp_normAccel;

bool redAlliance = false; //set later

void LED(char color) {
  switch (color) {
    case 'r':
      digitalWrite(redPin, HIGH);
      digitalWrite(greenPin, LOW);
      digitalWrite(bluePin, LOW);
      break;

    case 'g':
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, HIGH);
      digitalWrite(bluePin, LOW);
      break;

    case 'b':
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, LOW);
      digitalWrite(bluePin, HIGH);
      break;

    case 'w':
      digitalWrite(redPin, HIGH);
      digitalWrite(greenPin, HIGH);
      digitalWrite(bluePin, HIGH);
      break;
    case 'n': //nothing
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, LOW);
      digitalWrite(bluePin, LOW);
      break;
    default:
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, LOW);
      digitalWrite(bluePin, LOW);
      break;
  }
}


void Blink(int offTime , int onTime , char colors) {
    LED(colors);
    delay(onTime);
    LED('n');
    delay(offTime);
  }

void wake()
{
  angle = read_filtered_pitch(16) - pitch_offset;
  if (abs(angle) <= 8) Blink(200, 300, redAlliance ? 'r' : 'b');
  else Blink(200 , 8 ,  'g' );
  if (currentTime - wakeTime > 60000) Awake = false;
}

void sleep() 
{
  goto_sleep();
  turn_on_MPU();
  angle = read_filtered_pitch(10) - pitch_offset;
  if (abs(angle) >= 0.5)
  {
    wake_counter = 0;
    if (enable_wake)
    {
      Awake = true;
      enable_wake = false;
      wakeTime = currentTime;
    }
  }
  else
  { 
    wake_counter ++;
    if (wake_counter >2)
    {
      enable_wake = true;
      wake_counter = 0;
    }
  }
  
  // calibrate the 0 if static for a long time with small angle, probably drifted
  if (abs(angle - prev_angle)<0.3 && abs(angle)<2)
  {
    calib_counter ++;
    if (calib_counter > 120)  // more than 8 minutes
    {
      pitch_offset = read_filtered_pitch(100);  
      calib_counter = 0;
    }
  }
  else
  {
    calib_counter = 0;
  }
  prev_angle = angle;
}

float read_raw_pitch()
{
  normAccel = mpu.readNormalizeAccel();
  return (atan2(normAccel.XAxis, normAccel.YAxis) * 180.0) / M_PI;
}

float read_filtered_pitch(int n)
{
  float accum = 0;
  for (int i = 0 ; i < n; i++)
  {
    accum += read_raw_pitch();
    delay(10);
  }
  return (accum / n);
}


void goto_sleep()
{
  digitalWrite (MPUPin, LOW);
  LowPower.powerDown (SLEEP_4S, ADC_OFF, BOD_OFF);
  return;
}

void turn_on_MPU()
{
  digitalWrite (MPUPin, 1);
  LED(enable_wake ? 'g' : 'b');
  delay (5);
  LED('n');
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    delay(10);
  }
  for (int j = 0; j < 2; j++)
  {
    temp_normAccel = mpu.readNormalizeAccel();
    delay (10);
  }
  return;
}


void setup() {
  // put your setup code here, to run once:
  Awake = false;
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode (MPUPin, OUTPUT);
//  Serial.begin(57600);
  turn_on_MPU();
  delay(2000);
  pitch_offset = read_filtered_pitch(100);
}

void loop() {
  currentTime = millis();
  if (Awake)
  {
    wake();
  } 
  else 
  {
    sleep();
  }
}
