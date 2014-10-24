import serial
import serial.tools.list_ports
import sys
import win32api, win32con

ports = list(serial.tools.list_ports.comports())
for p in ports:
	print p
	if "Arduino Uno" in p[1]:
		ser = serial.Serial(p[0], 9600)
		break

if not 'ser' in locals().keys():
	print "Arduino not connected"
	sys.exit()

line = ser.readline().strip()

while line:
	print line
	if line=="up" :
		win32api.keybd_event(0x26, 0, 0, 0)		#up arrow
		win32api.keybd_event(0x26, 0, win32con.KEYEVENTF_KEYUP, 0)
	elif line=="down" :
		win32api.keybd_event(0x28, 0, 0, 0)		#down arrow
		win32api.keybd_event(0x28, 0, win32con.KEYEVENTF_KEYUP, 0)
	elif line=="left" :
		win32api.keybd_event(0x25, 0, 0, 0)		#left arrow
		win32api.keybd_event(0x25, 0, win32con.KEYEVENTF_KEYUP, 0)
	elif line=="right" :
		win32api.keybd_event(0x27, 0, 0, 0)		#right arrow
		win32api.keybd_event(0x27, 0, win32con.KEYEVENTF_KEYUP, 0)
	line = ser.readline().strip()