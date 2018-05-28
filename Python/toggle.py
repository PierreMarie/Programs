import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

GPIO.setup(17, GPIO.OUT)

i=1

while 1:
	GPIO.output(17, GPIO.HIGH)
	GPIO.output(17, GPIO.LOW)
	time.sleep(i)
	if i > 0.1:
		i=i-0.01
	else:
		i=1

#print("\n")
#print(time.time())
#print("\n")
