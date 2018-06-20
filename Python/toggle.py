import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

GPIO.setup(19, GPIO.OUT)

i=1

while 1:
	GPIO.output(19, GPIO.HIGH)
	time.sleep(0.0001)
	GPIO.output(19, GPIO.LOW)
	time.sleep(0.001)
	#if i > 0.1:
	#	i=i-0.01
	#else:
	#	i=1

#print("\n")
#print(time.time())
#print("\n")
