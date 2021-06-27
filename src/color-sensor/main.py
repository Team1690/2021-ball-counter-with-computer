import RPI.GPIO as GPIO
import time

S2_PIN = 0  # change later
S3_PIN = 0  # change later
OUTPUT_PIN = 0  # change later
CYCLES_TO_WAIT = 10


def ditect_color(S2_VALUE, S3_VALUE):
    GPIO.output(S2_PIN, S2_VALUE)
    GPIO.output(S3_PIN, S3_VALUE)
    time.sleep(0.3)
    start = time.time()
    for impulse_count in range(CYCLES_TO_WAIT):
        GPIO.wait_for_edge(OUTPUT_PIN, GPIO.FALLING)
    duration = time.time() - start  # seconds to run for loop
    red = CYCLES_TO_WAIT / duration  # in Hz
