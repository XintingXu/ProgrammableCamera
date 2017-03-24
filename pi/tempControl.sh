#! /bin/bash
for((i=0;i<30;i++))
do
	sudo python getTemp.py
	sleep 2
done