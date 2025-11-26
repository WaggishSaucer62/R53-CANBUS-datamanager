# R53-CANBUS-datamanager
## Intro:
An ESP32 based project to log and display information sourced from CANBUS messages from the R53 Mini Cooper S, using PlatformIO.
Note: I only have a few days of C++ outside of basic Arduino projects under my belt, so I'm sure there are many things that can be improved in the code, but at least it's better than whatever that 700 line Arduino file I had before was :P


## TODO:
 - [x] Refactor to use classes and PlatformIO.
 - [ ] Add SD card support.
 - [ ] Make HP estimation actually accurate.
 - [ ] See if it is possible to make it automatically sleep and wake from CAN signals.
 - [ ] See if it is possible to provide compiled code to make it easier to flash to ESP32.
 - [ ] Complete and upload PCBs.
 - [ ] Complete and upload 3D printed models for housings and mounts.
 - [ ] Scan for other dataframes and attempt to decode them.


## Installation:
 - Install Visual Studio Code.
 - Add the PlatformIO plugin.
 -  Download the full repo.
 -  In VS code, select the PlatformIO plugin in the sidebar, click add folder, and select the folder containing the whole repo, it should be able to recognise the platformio.ini file inside.
 -  Using the PlatformIO controls at the bottom, click upload (the arror) to build and flash the program to the ESP32 connected to your computer with a USB cable, ensure the cable is capable of data communication.
 -  If required, you can manually select the COM port of your microcontroller by clicking the plug icon in the bottom bar.
