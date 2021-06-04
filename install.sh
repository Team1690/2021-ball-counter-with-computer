#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
INSTALL_DIR=/home/pi/Documents/Cheesy-Arena

mkdir -p $INSTALL_DIR
cp $SCRIPT_DIR/deps/open-chrome.py $INSTALL_DIR
cp $SCRIPT_DIR/deps/start-cheesy-arena.sh $INSTALL_DIR
cp $SCRIPT_DIR/src/fms_communication.py $INSTALL_DIR

sudo cp $SCRIPT_DIR/deps/cheezy-arena-rs-client.desktop /etc/xdg/autostart
$INSTALL_DIR/start-cheesy-arena.sh

