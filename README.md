# -ESPhome-APATOR-meter-reader-

Measuring water consumption from a meter with Apator 16-2 interface using esphome simplicity.

Water meter reading solution with Apator interface, without installing wmbusmeters. All you need is ESP826 and CC1101 radio chip. 
On base: 
https://github.com/SzczepanLeon/wMbus-lib
https://github.com/olekdata/ESP8266-D1-Mini-Apator

File: apator.h, place in esphome/custom_components/ directory. 
Copy the data from the apator-meter.yaml file to the .yaml file generated in esphome's own sensor....
