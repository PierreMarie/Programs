#!/bin/python

import bluetooth
import time
import sys

bd_addr = "98:D3:31:30:43:73"
port = 1

state = 0
debut = 0
envoi_reussi = 0

try:
    while 1:
        if state == 0:
            try:
                sock = bluetooth.BluetoothSocket (bluetooth.RFCOMM)
                sock.connect((bd_addr, port))
                state = 1
                debut = 1

            except bluetooth.btcommon.BluetoothError:
		sock.close()
                state = 0
                print("Hote inatteignable")

        if state == 1:

            if debut == 1:
                print('Hote connecte')
                debut = 0

            try:
                from time import gmtime, strftime

		# SUMMER
                #h=int(strftime("%H", gmtime()))+2
                #m=int(strftime("%M", gmtime()))
                #s=int(strftime("%S", gmtime()))

		#if h == 24:
		#    h = 0

		#if h == 25:
		#	h = 1

		# WINTER
		h=int(strftime("%H", gmtime()))+1
                m=int(strftime("%M", gmtime()))
                s=int(strftime("%S", gmtime()))

		if h == 24:
		    h = 0


                sock.send("%s%s:%s:%s" % ("h","{0:0>2}".format(h),"{0:0>2}".format(m),"{0:0>2}".format(s)))
	        envoi_reussi = 1
		#sock.recv(1024)

            #except bluetooth.btcommon.BluetoothError:
            except:
	        sock.close()
                state = 0
                envoi_reussi = 0
                print("Hote deconnecte")

            if envoi_reussi == 1:
                print("Commande envoyee")
                envoi_reussi = 0

        time.sleep(60)

except KeyboardInterrupt:       # If CTRL+C is pressed, exit cleanly:
    sock.close()
    print("\nA bientot !")
