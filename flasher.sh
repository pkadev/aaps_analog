#!/bin/sh 
avrdude -c avrispv2 -p m48 -P usb -U flash:w:main.hex
