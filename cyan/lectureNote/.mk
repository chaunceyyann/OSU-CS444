#!/bin/sh
make && ~/git/pushbullet-bash/pushbullet push all file $1 && make clean
