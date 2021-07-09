#!/bin/sh

(python3 /home/pi/Documents/Cheesy-Arena/main.py password color > /dev/null 2>&1 & )
OPEN_CHROME=/home/pi/Documents/Cheesy-Arena/open-chrome.py
if [ -f $OPEN_CHROME ]
then
    (python3 $OPEN_CHROME > /dev/null 2>&1 & )
fi

