#!/bin/bash

HELP="USAGE: ./install.sh red/blue"
if [ $# -lt 1 ]
then
    echo $HELP
    exit
fi

if [ $1 != 'red' ] && [ $1 != 'blue']
then
    echo $HELP
    exit
fi

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
INSTALL_DIR=/home/pi/Documents/Cheesy-Arena

mkdir -p $INSTALL_DIR
cp $SCRIPT_DIR/deps/open-chrome.py $INSTALL_DIR
cp $SCRIPT_DIR/deps/start-cheesy-arena.sh $INSTALL_DIR
cp $SCRIPT_DIR/src/fms_communication.py $INSTALL_DIR
COLOR=$1
sed -i -e "s/color/$COLOR/g" $INSTALL_DIR/start-cheesy-arena.sh

sudo cp $SCRIPT_DIR/deps/cheezy-arena-rs-client.desktop /etc/xdg/autostart
$INSTALL_DIR/start-cheesy-arena.sh

