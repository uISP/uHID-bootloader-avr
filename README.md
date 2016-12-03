# uHID bootloader for AVR (vusb stack)

This is the uHID bootloader for AVR microcontrollers. Features:

- Follows uHID version 1 spec
- Includes flash and eeprom handling with readback support
- Configurable

# Compiling

- Get and install cmake
- Make sure you have all the avr tools in your path
```
mkdir build
cd build
cmake ..
make
```

#Adding a device

Just drop a file with configuration into devices/ directory.
Use existing files in that directory as a reference

#License

GPLv2 + obdev.at exception

See packages/vusb-20121206/License.txt

Since this bootloader uses vusb the licensing terms stay pretty much the same

#Authors

Andrew 'Necromant' Anrianov <ncrmnt.org>
