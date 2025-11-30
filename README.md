# R53-CANBUS-datamanager
## Intro:
An ESP32 based project to log and display information sourced from CANBUS messages from the R53 Mini Cooper S, using PlatformIO.

Note: I only have a few days of C++ experience outside of basic Arduino projects and I've never used classes before, so I'm sure there are many things that can be improved in the code, but at least it's better than whatever that 700 line Arduino file I had before was :P
***
**Please see the wiki tab at the top of the repo for more detailed instructions, but a basic overview can be found below.**
***


## TODO:
 - [x] Refactor to use classes and PlatformIO.
 - [ ] Add SD card support.
 - [ ] Make HP estimation actually accurate.
 - [ ] See if it is possible to make it automatically sleep and wake from CAN signals.
 - [x] See if it is possible to provide compiled code to make it easier to flash to ESP32.
 - [ ] Complete and upload PCBs.
 - [ ] Complete and upload 3D printed models for housings and mounts.
 - [ ] Scan for other dataframes and attempt to decode them.
 - [ ] Add wiring diagram for car cable splitter to README.
 - [ ] Add ARGB strip functionality


## Parts List:
### Main electronics
 - 38 pin ESP32 dev module
 - MCP2515 CAN transciever
 - 240x320 2.8" SPI TFT LCD Touch screen with ILI9341 driver and built in SD card reader
 - 12v to 5v buck converter if required
 - PCB has a port for an ARGB strip, I will use a WS2812B controlled one, but the codebase does not yet use it.
 - Main PCB

### Cable to connect to car
It just needs access to CANBUS wires and power from anywhere, as I did not want to damage the original wiring, I created a custom cable that goes inbetween the stock cable and the tacho, splitting off 12v power and CANBUS high/low
The connector to look for is "IL-AG5-7S-S3C1", this is the listing I bought it from:
 - https://www.aliexpress.com/item/1005004188142201.html?spm=a2g0o.order_list.order_list_main.34.25871802BuPLSL


## Known Issues:
For some reason, if the car 12v power is connected to the module, **remote locking for the car works intermittently**, but does not affect key based locking. The issue occurs with either power and data, or just power wires plugged in, I have not tested data only.


## Installation:
Throughout both methods, when flashing to the ESP32, you may need to press the boot button (right side when looking from the usb port side) to help it connect.
### From Source:
 - Install Visual Studio Code.
 - Add the PlatformIO plugin.
 -  Download the full repo.
 -  In VS code, select the PlatformIO plugin in the sidebar, click add folder, and select the folder containing the whole repo, it should be able to recognise the platformio.ini file inside.
 -  Using the PlatformIO controls at the bottom, click build to automatically download the required libraries.
 -  Go to /.pio/libdeps/esp32dev/TFT_eSPI and copy in User_Setup.h, to import the correct settings for the display used.
 -  Using the PlatformIO controls again, click upload (the arrow) to build and flash the program to the ESP32 connected to your computer with a USB cable, ensure the cable is capable of data communication.
 -  If required, you can manually select the COM port of your microcontroller by clicking the plug icon in the bottom bar.

### From Build Files:
 - Install python
 - Run `pip install esptool`
 - Download the bootloader.bin, partitions.bin, and firmware.bin files from the latest release.
 - Find the COM port of the esp32, on Windows, this can be done by right clicking the Windows icon in the bottom left, clicking device manager, and opening "Ports (COM & LPT)" and looking for the device that corresponds to your ESP32 to find the COM address, if you don't know by the name of it, you can unplug it to see which one dissapears from the list.
 - Run `esptool --port YOURESP32COMPORT erase_flash`, for example `esptool --port COM4 erase_flash`
 - Run `esptool --chip esp32 --port YOURESP32COMPORT --baud 460800 write_flash -z 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin` in the same folder as the downloaded files.
