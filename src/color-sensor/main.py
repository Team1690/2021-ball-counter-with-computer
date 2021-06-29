import RPI.GPIO as GPIO
import time

S2_PIN = 0  # change later
S3_PIN = 0  # change later
OUTPUT_PIN = 0  # change later
CYCLES_TO_WAIT = 10


def get_raw_frequency(S2_VALUE, S3_VALUE):
    GPIO.output(S2_PIN, S2_VALUE)
    GPIO.output(S3_PIN, S3_VALUE)
    time.sleep(0.3)
    start = time.time()
    for impulse_count in range(CYCLES_TO_WAIT):
        GPIO.wait_for_edge(OUTPUT_PIN, GPIO.FALLING)
    duration = time.time() - start  # seconds to run for loop
    frequency = CYCLES_TO_WAIT / duration  # in Hz
    return frequency

def setup():
  GPIO.setmode(GPIO.BCM)
  GPIO.setup(OUTPUT_PIN,GPIO.IN, pull_up_down=GPIO.PUD_UP)
  GPIO.setup(S2_PIN,GPIO.OUT)
  GPIO.setup(S3_PIN,GPIO.OUT)
  print("\n")

if __name__=='__main__':
    setup()
    while(True):

        red_frequncy = detect_color(GPIO.LOW, GPIO.LOW)
        blue_frequncy = detect_color(GPIO.LOW, GPIO.HIGH)
        green_frequncy = detect_color(GPIO.HIGH, GPIO.HIGH)