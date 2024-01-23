#!/bin/bash

# Prepare M5StackC2
cd ..
#rm -rf build
mkdir build
cd build
cmake -DBOARD=M5STACKCORE2 ../
make -j 12
make spiffs_spiffs_bin
cd ../scripts
mkdir M5StackCore2
cp ../build/ota_data_initial.bin ./M5StackCore2/
cp ../build/partition_table/partition-table.bin ./M5StackCore2/
cp ../build/bootloader/bootloader.bin ./M5StackCore2/
cp ../build/homepoint.bin ./M5StackCore2
cp ../build/spiffs.bin ./M5StackCore2
cd M5StackCore2
python2 ../merge_bin_esp.py --output_name homepoint_m5stackcore2_full.bin --bin_path partition-table.bin ota_data_initial.bin bootloader.bin homepoint.bin spiffs.bin --bin_address 0x8000 0xd000 0x1000 0x10000 0x2b0000
cd ..
mkdir release
cp ./M5StackCore2/output/homepoint_m5stackcore2_full.bin ./release
cp ./M5StackCore2/homepoint.bin ./release/homepoint_m5stackcore2_ota_update.bin


cp readme.txt ./release
zip -r homepoint_release.zip ./release
