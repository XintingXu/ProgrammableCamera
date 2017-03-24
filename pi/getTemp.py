import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD)

GPIO.setup(29,GPIO.OUT)

file = open("/sys/class/thermal/thermal_zone0/temp")
temp = float(file.read()) / 1000.00
file.close()
print "temp : %.2f" %temp
if(temp > 50):
	GPIO.output(29,GPIO.HIGH)
if(temp < 40):
	GPIO.output(29,GPIO.LOW)