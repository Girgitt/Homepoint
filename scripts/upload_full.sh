#!/bin/bash
sudo esptool -b 921600 write_flash -fs 4MB -fm dout -z 0x0 ./M5StackCore2/output/homepoint_m5stackcore2_full.bin
minicom -b 115200 -D /dev/ttyUSB0
