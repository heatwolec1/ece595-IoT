#!/bin/bash

LOG_FILE=temperature-humidity.csv

echo -ne "First measurement: \n\t"
head -n 1 $LOG_FILE

echo -ne "Last measurement: \n\t"
tail -n 1 $LOG_FILE

echo -ne "Bytes required for each measurement: \n\t"
wc -L $LOG_FILE | cut -d " " -f 1 -

echo -ne "Total number of measurements: \n\t"
wc -l $LOG_FILE | cut -d " " -f 1 -

echo -ne "File size: \n\t"
du -h $LOG_FILE | cut -f 1 -
