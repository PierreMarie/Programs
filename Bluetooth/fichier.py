#!/bin/python
import time

#contenu = mon_fichier.read()
#print(contenu)

temperature = "25.4 oC"

try:
    while 1:
	
		mon_fichier = open("/var/www/html/serveur_1/value.txt", "w")

		from time import gmtime, strftime

		h=int(strftime("%H", gmtime()))+2

		if h == 24:
		    h = 0

		if h == 25:
			h = 1

		m=int(strftime("%M", gmtime()))
		s=int(strftime("%S", gmtime()))

		mon_fichier.write(strftime("%a, %d %b %Y", gmtime()))
		mon_fichier.write("\t%s:%s:%s" % ("{0:0>2}".format(h),"{0:0>2}".format(m),"{0:0>2}".format(s)))
		mon_fichier.write("\n%s" % temperature)

		time.sleep(1)

except KeyboardInterrupt:       # If CTRL+C is pressed, exit cleanly:
    mon_fichier.close()
    print("\nA bientot !")	
