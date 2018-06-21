import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

GPIO.setup(19, GPIO.IN)

def my_callback(channel):
    global prev
    curre = time.time()
    if 'prev' in globals():
        temp=1/(curre-prev)
	print('%.1f Hz' % temp)
    
    prev = curre

#GPIO.add_event_detect(26, GPIO.RISING, callback=my_callback, bouncetime=1)
GPIO.add_event_detect(19, GPIO.RISING, callback=my_callback)

try:
    while 1:
        time.sleep(1)

	# GPIO.wait_for_edge(channel, GPIO.RISING)

except KeyboardInterrupt: 	# If CTRL+C is pressed, exit cleanly:
    GPIO.cleanup() 		# cleanup all GPIO
