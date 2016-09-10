#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys
import time
import RPi.GPIO as GPIO
from pygame import mixer

# Global is bad, but it's work.
my_gpio = 21
nb_tours = 0

def play_sound(wav_file):
    ''' Play wav_file on audio output '''
    print "Play sound"
    mixer.init()
    sound = mixer.Sound(wav_file)
    sound.play()
    time.sleep(5)
    return

def tours_to_action(tours):
    ''' nb tours to action '''
    # todo: make an array nb_tours
    if tours == 50000:
        play_sound("./01.wav")
        return 0
    return tours
    
def read_gpio(my_gpio):
    ''' read my_gpio port and play sound when sensor is enable '''
    global nb_tours
    GPIO.setmode(GPIO.BCM)        # Pin set in GPIO model
    GPIO.setup(my_gpio, GPIO.IN)  # Setup pin in readable mode
    try:
        while True:
            print nb_tours
            # If sensor is enable
            if not GPIO.input(my_gpio):
                nb_tours += 1
            nb_tours = tours_to_action(nb_tours)
    # Catch ctrl+c to quit properly
    except KeyboardInterrupt:
        GPIO.cleanup()
    return

if __name__ == '__main__':
    ''' Original Entry Point '''
    read_gpio(my_gpio)
