import telnetlib
import win32api, win32con

# tn = telnetlib.Telnet('192.168.1.168', 2000)
tn = telnetlib.Telnet('1.2.3.4', 2000)
line = tn.read_some().strip()

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
	elif line=="buttonA" :
		win32api.keybd_event(0x52, 0, 0, 0)		#r
		win32api.keybd_event(0x52, 0, win32con.KEYEVENTF_KEYUP, 0)
	line = tn.read_some().strip()