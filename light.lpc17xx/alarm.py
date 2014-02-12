#!/usr/bin/python
import time;  
import serial;

ser = serial.Serial('/dev/tty.usbmodem1a1221')

alarmMin = 1
alarmHour = 7

while 1:
	time.sleep(5)
	x = time.localtime()
	print x
	if x[3] == alarmHour:
		if x[4] == alarmMin:
			ser.write("o\n")
		


