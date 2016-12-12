#include <avr/boot.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <usbconfig.h>
#include <usbdrv/usbportability.h>
#include <usbdrv/usbdrv.h>
#include <uhid.h>

#ifndef GICR
#define GICR    MCUCR
#endif

static void reconnect()
{
	DDRD |= (1<<CONFIG_USB_CFG_DMINUS_BIT) | (1<<CONFIG_USB_CFG_DMINUS_BIT);
	PORTD &= ~((1<<CONFIG_USB_CFG_DMINUS_BIT) | (1<<CONFIG_USB_CFG_DMINUS_BIT));
	_delay_ms(5);
	DDRD &= ~((1<<CONFIG_USB_CFG_DMINUS_BIT) | (1<<CONFIG_USB_CFG_DMINUS_BIT));
}

#include "bootlogic.c"

const PROGMEM char usbHidReportDescriptor[42] = {
	0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
	0x09, 0x01,                    // USAGE (Vendor Usage 1)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
	0x75, 0x08,                    //   REPORT_SIZE (8)

	0x85, 0x01,                    //   REPORT_ID (1)
	0x95, IOBUFLEN,                    //   REPORT_COUNT (255)
	0x09, 0x00,                    //   USAGE (Undefined)
	0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)

	0x85, 0x02,                    //   REPORT_ID (2)
	0x95, IOBUFLEN,                    //   REPORT_COUNT (6)
	0x09, 0x00,                    //   USAGE (Undefined)
	0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)

	0x85, 0x03,                    //   REPORT_ID (3)
	0x95, IOBUFLEN,                    //   REPORT_COUNT (131)
	0x09, 0x00,                    //   USAGE (Undefined)
	0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)

	0xc0                           // END_COLLECTION
};


/* We won't use antares startup to save a few bytes */
int main()
{
	GICR = (1 << IVCE) ;  /* enable change of interrupt vectors */
	GICR = (1 << IVSEL); /* move interrupts to boot flash section */
	initRunButton();
	reconnect();

	sei();
  	usbInit();

#ifdef CONFIG_RUN_BUTTON_ON_START
	if (checkRunButton())
		leaveBootloader();
#endif

	while (1) {
		usbPoll();

#ifdef	CONFIG_RUN_BUTTON_ON_LOOP
		if (checkRunButton())
			leaveBootloader();
#endif
	}
}

/* Shut up a vusb warning when including */
USB_PUBLIC usbMsgLen_t usbFunctionDescriptor(struct usbRequest *rq)
{

}
#include "../packages/vusb-20121206/usbdrv/usbdrv.c"
