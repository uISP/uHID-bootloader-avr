# Chose between smaller size or faster flashing speed
# e.g. "small" or "fast"
set(UHID_USB_STACK lufa)

set(CONFIG_UHID_VARIANT "fast")

#Disable eeprom hadling to save up some flash
set(CONFIG_UHID_EEPROM YES)
set(CONFIG_UHID_EEPROM_READBACK YES)

#When to check the run button state
#start, loop, none
set(CONFIG_RUN_BUTTON "loop")
set(CONFIG_RUN_BUTTON_IOPORT E)
set(CONFIG_RUN_BUTTON_BIT  2)

# Hardware config
set(CONFIG_MCU  atmega32u4)
set(CONFIG_LOAD_ADDR 0x7000)

set(CONFIG_F_CPU  16000000)

#Device descriptors tuning
#Words waste flash
set(CONFIG_USB_VID   1d50)
set(CONFIG_USB_PID   6032)

set(CONFIG_USB_SERIAL   PowerHub)
set(CONFIG_USB_PRODUCT  ${CONFIG_MCU})
set(CONFIG_USB_VENDOR   uHID)

#Buildsystem
set(CONFIG_FILENAME uhid-${CONFIG_USB_PRODUCT}-${CONFIG_USB_SERIAL}-${CONFIG_F_CPU})
