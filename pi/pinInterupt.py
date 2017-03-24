import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BOARD)
GPIO.setup(40,GPIO.IN,pull_up_down=GPIO.PUD_UP)
GPIO.setup(29,GPIO.OUT)
while 1:
        try:
            GPIO.wait_for_edge(40,GPIO.FALLING)
            GPIO.output(29,GPIO.HIGH)
        except KeyboardInterrupt:
            GPIO.cleanup()
            quit()
GPIO.cleanup()
