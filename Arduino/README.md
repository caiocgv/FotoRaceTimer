# Foto Race Timer
## Intro
Foto Race timer is a millisecond precision timer based on two RTC modules sycronized that will measure the time between an start and a finish.
## Hardware
Arduino based, will be improved to esp8266 for wifi capability and later to esp32 LoRa for radio com between modules.
LDR for passage detection
LED for status and feeding LDR
SD card module for data storage (will be removed when ESP modules are implemented)
RTC module for time keeping
## Software
Arduino software is pretty straight forward with few commands that the user can iterate
#### Delete
Read all stored data from previous passages and delete the file (the file cannot be recovered)
#### Read
Read all stored data
#### ID input
After every passage an ID (unique number or name) should be input in order for the time to be stored.
## Data Processing
With the data collected (ID, start, end) it should be imported with an custom EXCEL SHEET script that will handle the math and return the rank. .bas file is available here. Will be removed after transfering to python and pandas table.
