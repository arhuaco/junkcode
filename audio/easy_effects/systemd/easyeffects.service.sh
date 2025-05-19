#!/bin/bash

if [ "$1" = "start" ]; then
	pkill Xvfb
	sleep 1
	Xvfb :43 -screen 0 1024x768x16 &
	sleep 3
	export DISPLAY=:43
	nohup easyeffects --gapplication-service  2>/dev/null >/dev/null &
fi

if [ "$1" = "stop" ]; then
	easyeffects --quit
	pkill Xvfb
fi

