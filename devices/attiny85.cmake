#Warning: No support for attiny85 yet, WiP, just a test

set(UHID_USB_STACK vusb)
set(CONFIG_UHID_VARIANT "fast")

#Disable eeprom hadling to save up some flash
set(CONFIG_UHID_EEPROM YES)
set(CONFIG_UHID_EEPROM_READBACK YES)

#When to check the run button state
#start, loop, none
set(CONFIG_RUN_BUTTON "none")
set(CONFIG_RUN_BUTTON_IOPORT C)
set(CONFIG_RUN_BUTTON_BIT  1)

# Hardware config
set(CONFIG_MCU  attiny85)
set(CONFIG_LOAD_ADDR 0x1800)
set(CONFIG_F_CPU  12000000)

set(CONFIG_USB_IOPORT B)
set(CONFIG_USB_DPLUS_BIT 1)
set(CONFIG_USB_DMINUS_BIT  2)


#Device descriptors tuning
#Words waste flash
set(CONFIG_USB_VID   1d50)
set(CONFIG_USB_PID   6032)

set(CONFIG_USB_SERIAL   Test)
set(CONFIG_USB_PRODUCT  ${CONFIG_MCU})
set(CONFIG_USB_VENDOR   uHID)

#Buildsystem
set(CONFIG_FILENAME uhid-${CONFIG_USB_PRODUCT}-${CONFIG_USB_SERIAL}-${CONFIG_F_CPU})
