import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

GPIO.setup(18, GPIO.OUT)
GPIO.setup(17, GPIO.IN)

def my_callback(channel):
	global prev
	curre = time.time()
	if 'prev' in globals():
		#print("\n")
		temp=1/(600*(curre-prev))
		print('%.1f Hz' % temp)
	prev = curre

	#print(time.time())
	#print("\n")

GPIO.add_event_detect(17, GPIO.RISING, callback=my_callback, bouncetime=1)

try:
    while 1:
	time.sleep(1)

#		GPIO.output(18, GPIO.HIGH)

#		if GPIO.input(17):
#			print("Pin 17 is HIGH")

#		else:
#			print("Pin 17 is LOW")

#		time.sleep(0.2)

		# GPIO.wait_for_edge(channel, GPIO.RISING)

except KeyboardInterrupt: 	# If CTRL+C is pressed, exit cleanly:
	GPIO.cleanup() 			# cleanup all GPIO
