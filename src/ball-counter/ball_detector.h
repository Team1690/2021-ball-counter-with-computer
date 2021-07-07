#include <Arduino.h>

class Ball_detector
{
    const int pin_number;
    static const int THRESHOLD_HIGH = 600;
    static const int THRESHOLD_LOW = 400;
    bool curr_state = false; //true=sensing ball false=not sensing ball
    bool last_state = false;
    int sensor_val;
    bool ball_found = false;

public:
    Ball_detector(const int pin_number) : pin_number(pin_number) {}

    bool ball_detected()
    {
        sensor_val = analogRead(pin_number);

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
            ball_found = true;
        }
        else
        {
            ball_found = false;
        }

        last_state = curr_state;

        return ball_found;
    }

    int get_value()
    {
        return sensor_val;
    }
};
