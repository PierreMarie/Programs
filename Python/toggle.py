import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

GPIO.setup(25, GPIO.OUT)

i=1

while 1:
	GPIO.output(25, GPIO.HIGH)
	GPIO.output(25, GPIO.LOW)
	time.sleep(0.01)
	#if i > 0.1:
	#	i=i-0.01
	#else:
	#	i=1

#print("\n")
#print(time.time())
#print("\n")
