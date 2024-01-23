#!/bin/bash
sudo esptool -b 921600 write_flash -fs 4MB -fm dout -z 0x10000 ./M5StackCore2/homepoint.bin
minicom -b 115200 -D /dev/ttyUSB0
