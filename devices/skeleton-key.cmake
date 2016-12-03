

#loader functionality
set(CONFIG_UHID_EEPROM YES)
set(CONFIG_UHID_EEPROM_READBACK NO)

#When to check the run button state
#start, loop, none
set(CONFIG_RUN_BUTTON "start")
set(CONFIG_RUN_BUTTON_PORT C)
set(CONFIG_RUN_BUTTON_PIN  2)

# Hardware config
set(CONFIG_MCU  atmega8)
set(CONFIG_LOAD_ADDR 0x1800)
set(CONFIG_F_CPU  12000000)

set(CONFIG_USB_IOPORT D)
set(CONFIG_USB_DPLUS_BIT 3)
set(CONFIG_USB_DMINUS_BIT  2)


#Device descriptors tuning
set(CONFIG_USB_VID   1d50)
set(CONFIG_USB_PID   6032)

set(CONFIG_USB_SERIAL   SkeletonKey)
set(CONFIG_USB_PRODUCT  ${CONFIG_MCU})
set(CONFIG_USB_VENDOR   uHID)

#Buildsystem
set(CONFIG_FILENAME uhid-${CONFIG_USB_PRODUCT}-${CONFIG_USB_SERIAL}-${CONFIG_F_CPU})
