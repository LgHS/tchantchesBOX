#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys
import time
import RPi.GPIO as GPIO
from pygame import mixer

# Global is bad, but it's work.
my_gpio = 4
last_activity = 0

def is_the_good_time():
    ''' when do you play sound ? '''
    global last_activity
    now = int(time.time())

    if ((now - last_activity) >= 1*60):
        play_sound("./01.wav")
    last_activity = now
    return

def play_sound(wav_file):
    ''' Play wav_file on audio output '''
    print "Play sound"
    mixer.init()
    sound = mixer.Sound(wav_file)
    sound.play()
    time.sleep(5)
    return

def read_gpio(my_gpio):
    ''' read my_gpio port and play sound when sensor is enable '''
    GPIO.setmode(GPIO.BCM)        # Pin set in GPIO model
    GPIO.setup(my_gpio, GPIO.IN)  # Setup pin in readable mode
    try:
        while True:
            # If sensor is enable
            if GPIO.input(my_gpio):
                print "High"
                is_the_good_time()
            # One second of delay
            time.sleep(1)
    # Catch ctrl+c to quit properly
    except KeyboardInterrupt:
        GPIO.cleanup()
    return

if __name__ == '__main__':
    ''' Original Entry Point '''
    read_gpio(my_gpio)
