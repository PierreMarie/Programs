#!/bin/bash
while [ true ];do
	sudo rfcomm connect 0 98:D3:31:30:43:73 1&
	sudo echo `date '+%T'` > /dev/rfcomm0
	sleep 5
done