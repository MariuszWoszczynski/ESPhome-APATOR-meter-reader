# -ESPhome-APATOR-meter-reader-

Measuring water consumption from a meter with Apator 16-2 interface using esphome simplicity.

Water meter reading solution with Apator interface, without installing wmbusmeters. All you need is ESP826 and CC1101 radio chip. 
On base: 
https://github.com/SzczepanLeon/wMbus-lib
https://github.com/olekdata/ESP8266-D1-Mini-Apator

1. File: apator.h, place in esphome/custom_components/ directory. 
2. Config CC1101 and your water meter ID in apator.h file
3. Copy the data from the apator-meter.yaml file to the .yaml file generated in esphome's own sensor....
