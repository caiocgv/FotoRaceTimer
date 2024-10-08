# Foto Race Timer
## Intro
This is the Arduino Based Setup.
## Hardware
* Arduino nano
* LDR for passage detection
* LED for status
* Laser Diode for feeding LDR
* SD card module for data storage
* RTC module for time keeping
## Software
Arduino software is pretty straight forward. The passage is detected due to the variation of the resistance on the LDR and the current time is registered. Millisecond is not native and is simulated by arduino based on second change. With few commands that the user can iterate
#### Delete
Read all stored data from previous passages and delete the file (the file cannot be recovered)
#### Read
Read all stored data
#### ID input
After every passage an ID (unique number or name) should be input in order for the time to be stored.
## Data Processing
#### Data handling 
Specific Apps for Android should be installed to register athlete IDs and Read/delete data. OTG cable needed.
* [Serial Bluetooth](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal) write only
* [Serial USB](https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal) write only
* [Arduino Droid](https://play.google.com/store/apps/details?id=name.antonsmirnov.android.arduinodroid2) write/read/delete
#### Data Processing
* With the data collected (ID, start, end) it should be imported with an custom EXCEL SHEET script that will handle the math and return the rank. 
* Python web app coming next release.
## Hardware schematic
![Schematic_arduino_fotocel_2024-09-20](https://github.com/user-attachments/assets/6f1753b8-dcc6-45f1-bae9-e66507c99d4a)
