# uHID bootloader for AVR (v-usb and lufa usb stacks)

This is the uHID bootloader for AVR microcontrollers. Features:

- Tiny size: Fits in 2KiB of flash if atmega8 with all features enabled
- Follows uHID version 1 spec
- Includes flash and eeprom handling with readback support
- Configurable (Disable features and save space!)
- OpenSource!

# Compiling

- Install git cmake and avr tools.
- Get the uHID bootloader code and submodules

```
git clone https://github.com/uISP/uHID-bootloader-avr.git
cd uHID-bootloader-avr
git submodule update --init
```

- Make sure you have all the avr tools in your path

```
mkdir build
cd build
cmake ..
make
```

#Adding a device

Just drop a file with configuration into devices/ directory.
Use existing targets as a reference
A typical configuration looks like this

```
#Pick a usb stak to use (vusb or lufa)
set(UHID_USB_STACK lufa)

# Chose between smaller size or faster flashing speed
# e.g. "small" or "fast"
set(CONFIG_UHID_VARIANT "fast")

#Disable eeprom hadling to save up some flash
set(CONFIG_UHID_EEPROM YES)
set(CONFIG_UHID_EEPROM_READBACK YES)

#When to check the run button state
#start, loop, none
set(CONFIG_RUN_BUTTON "start")
set(CONFIG_RUN_BUTTON_IOPORT C)
set(CONFIG_RUN_BUTTON_BIT  1)

# Hardware config
set(CONFIG_MCU  atmega8)
set(CONFIG_LOAD_ADDR 0x1800)
set(CONFIG_F_CPU  12000000)

# USB IO config (v-usb only)
set(CONFIG_USB_IOPORT D)
set(CONFIG_USB_DPLUS_BIT 3)
set(CONFIG_USB_DMINUS_BIT  2)


#Device descriptors tuning
#Words waste flash
set(CONFIG_USB_VID   1d50)
set(CONFIG_USB_PID   6032)

set(CONFIG_USB_SERIAL   SKey)
set(CONFIG_USB_PRODUCT  ${CONFIG_MCU})
set(CONFIG_USB_VENDOR   uHID)

#Buildsystem
set(CONFIG_FILENAME uhid-${CONFIG_USB_PRODUCT}-${CONFIG_USB_SERIAL}-${CONFIG_F_CPU})
```

#License

uHID bootloader uses 2 different usb stacks, depending on the device in use, therefore the licensing mess looks like this:

* Builds that use v-usb stack are subject to GPLv2 + obdev.at exception.
  See packages/v-usb/License.txt for details

* Builds that use LUFA share the very same permissive license.
  See packages/lufa/LUFA/License.txt for details

#Authors

Andrew 'Necromant' Anrianov <ncrmnt.org>
