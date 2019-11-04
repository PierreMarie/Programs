import time


try:
   while 1:
      time.sleep(1)
      print('OK !')

except KeyboardInterrupt: 	# If CTRL+C is pressed, exit cleanly:
   print('\nA+')