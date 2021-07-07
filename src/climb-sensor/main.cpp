#include <Wire.h>
#include <MPU6050.h>
#include <LowPower.h>

MPU6050 mpu;
#define MPUPIN 5
float pitch_offset = 0;
float angle;
bool Awake;
bool enable_wake = true;
char color;

double currentTime = 0;
double wakeTime = 0;

bool white;
Vector normAccel;
Vector temp_normAccel;

const int redPin = 3;
const int greenPin = 4;
const int bluePin = 2;
const int MPUPin = 5;

bool greenState;

bool redAlliance = true; //set later

void LED(char color)
{
  switch (color)
  {
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
    break;
  }
}

void Blink(int offTime, int onTime, char colors)
{
  LED(colors);
  delay(onTime);
  LED('n');
  delay(offTime);
}

float read_raw_pitch()
{
  normAccel = mpu.readNormalizeAccel();
  return (atan2(normAccel.XAxis, normAccel.YAxis) * 180.0) / M_PI;
}

void goto_sleep()
{
  digitalWrite(MPUPin, LOW);
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
  return;
}

void turn_on_MPU()
{
  digitalWrite(MPUPIN, 1);
  LED('g');
  delay(10);
  LED('n');
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(10);
  }
  for (int j = 0; j < 2; j++)
  {
    temp_normAccel = mpu.readNormalizeAccel();
    delay(10);
  }
  return;
}

float read_filtered_pitch(int n)
{
  float accum = 0;
  for (int i = 0; i < n; i++)
  {
    accum += read_raw_pitch();
    delay(10);
  }
  return (accum / n);
}

void wake()
{
  angle = read_filtered_pitch(25) - pitch_offset;
  if (currentTime - wakeTime > 60000)
  {
    Awake = false;
  }
  else
  {
    if (abs(angle) <= 8)
    {
      Blink(200, 400, redAlliance ? 'r' : 'b');
    }
    else
    {
      Blink(200, 8, 'g');
    }
  }
}

void sleep()
{
  goto_sleep();
  turn_on_MPU();
  angle = read_filtered_pitch(10) - pitch_offset;
  if (abs(angle) >= 0.5)
  {
    if (enable_wake)
    {
      Awake = true;
      enable_wake = false;
      wakeTime = currentTime;
    }
  }
  else
  {
    enable_wake = true;
  }
}

void setup()
{
  // put your setup code here, to run once:
  Awake = false;
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  Serial.begin(57600);

  pinMode(MPUPIN, OUTPUT);

  turn_on_MPU();
  pitch_offset = read_filtered_pitch(100);
}

void loop()
{
  // put your main code here, to run repeatedly:
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