#!/bin/bash

HELP="USAGE: ./install.sh color/balls fms_admin_password red/blue"
if [ $# -lt 3 ]
then
    echo $HELP
    exit
fi

SENSOR=$1
PASSWORD=$2
COLOR=$3

if [[ ($COLOR != 'red' && $COLOR != 'blue') || ($SENSOR != 'balls' && $SENSOR != 'color') ]]
then
    echo $HELP
    exit
fi


SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
INSTALL_DIR=/home/pi/Documents/Cheesy-Arena

mkdir -p $INSTALL_DIR

cp $SCRIPT_DIR/deps/start-cheesy-arena.sh $INSTALL_DIR
if [[ $SENSOR == 'balls' ]]
then
    cp $SCRIPT_DIR/deps/open-chrome.py $INSTALL_DIR
    cp $SCRIPT_DIR/src/ball-counter/fms_communication.py $INSTALL_DIR/main.py
elif  [[ $SENSOR == 'color' ]]
then
    cp $SCRIPT_DIR/src/color-sensor/main.py $INSTALL_DIR/main.py
fi

sed -i -e "s/color/$COLOR/g" $INSTALL_DIR/start-cheesy-arena.sh
sed -i -e "s/password/$PASSWORD/g" $INSTALL_DIR/start-cheesy-arena.sh

sudo cp $SCRIPT_DIR/deps/cheezy-arena-rs-client.desktop /etc/xdg/autostart
$INSTALL_DIR/start-cheesy-arena.sh

