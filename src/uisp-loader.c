#include <arch/antares.h>
#include <avr/boot.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <generated/usbconfig.h>
#include <arch/vusb/usbportability.h>
#include <arch/vusb/usbdrv.h>
#include "uisp.h"

static void (*nullVector)(void) __attribute__((__noreturn__));

#ifndef TCCR0
#define TCCR0   TCCR0B
#endif
#ifndef GICR
#define GICR    MCUCR
#endif

static void leaveBootloader()
{
	cli();
	USB_INTR_ENABLE = 0;
	USB_INTR_CFG = 0;       /* also reset config bits */
	TCCR0 = 0;              /* default value */
	GICR = (1 << IVCE);     /* enable change of interrupt vectors */
	GICR = (0 << IVSEL);    /* move interrupts to application flash section */
/* We must go through a global function pointer variable instead of writing
 *  ((void (*)(void))0)();
 * because the compiler optimizes a constant 0 to "rcall 0" which is not
 * handled correctly by the assembler.
 */
	nullVector();
}


#define UISP_PART_NAME_LEN  8
#define UISP_INFO_LEN   (sizeof(struct deviceInfo) + 2 * sizeof(struct partInfo))

struct partInfo {
	uint16_t      pageSize;
	uint32_t      size;
	uint8_t       ioSize;
	uint8_t       name[UISP_PART_NAME_LEN];
}  __attribute__((packed));

struct deviceInfo {
	uint8_t       reportId;
	uint8_t       numParts;
	uint8_t       cpuFreq;
	struct partInfo parts[];
} __attribute__((packed));


#define IOBUFLEN 64

#ifndef CONFIG_UHID_EEPROM_READBACK
	#define EEP_PART_NAME "eeprom/wo"
#else
	#define EEP_PART_NAME "eeprom"
#endif

static const PROGMEM struct deviceInfo devInfo = {
	.cpuFreq = F_CPU / 100000,

#ifdef CONFIG_UHID_EEPROM
	.numParts = 2,
#else
	.numParts = 1,
#endif

	.parts = {
		{SPM_PAGESIZE, CONFIG_AVR_BLDADDR, IOBUFLEN,       "flash"  },
#ifdef CONFIG_UHID_EEPROM
		{SPM_PAGESIZE, E2END + 1,          IOBUFLEN,       EEP_PART_NAME }
#endif
	},
};


const PROGMEM char usbHidReportDescriptor[42] = {
	0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
	0x09, 0x01,                    // USAGE (Vendor Usage 1)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
	0x75, 0x08,                    //   REPORT_SIZE (8)

	0x85, 0x01,                    //   REPORT_ID (1)
	0x95, 0x06,                    //   REPORT_COUNT (6)
	0x09, 0x00,                    //   USAGE (Undefined)
	0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)

	0x85, 0x02,                    //   REPORT_ID (2)
	0x95, 0x83,                    //   REPORT_COUNT (131)
	0x09, 0x00,                    //   USAGE (Undefined)
	0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)

	0x85, 0x03,                    //   REPORT_ID (3)
	0x95, 0x05,                    //   REPORT_COUNT (0x3)
	0x09, 0x00,                    //   USAGE (Undefined)
	0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)

	0xc0                           // END_COLLECTION
};

static char     target;
static uchar    replyBuffer[IOBUFLEN];
static uint8_t  wLength;

#if (((FLASHEND) > 0xffff) || ((E2END) > 0xffff))
typedef uint32_t addr_t;
#define do_memcpy(d,s,l) memcpy_PF(d,s,l)
#else
typedef uint16_t addr_t;
#define do_memcpy(d,s,l) memcpy_P(d,s,l)
#endif

static addr_t addr;

uchar   usbFunctionSetup(uchar data[8])
{
	usbRequest_t    *rq = (void *)data;
	/* Avoid pointer some pointer deferences, they eat up flash */
	target = rq->wValue.bytes[0];
	wLength = rq->wLength.bytes[0];
	uint8_t wValue = rq->wValue.bytes[0];
	uint8_t bRequest = rq->bRequest;

	if (bRequest == USBRQ_HID_SET_REPORT) {
		if (!wValue)
			leaveBootloader();
		else
			return USB_NO_MSG;
	} else if(bRequest == USBRQ_HID_GET_REPORT) {
		usbMsgPtr = replyBuffer;
		if (wValue==0) {
			do_memcpy(replyBuffer, &devInfo, UISP_INFO_LEN);
			addr = 0;
			return UISP_INFO_LEN;
		} else if (wValue == 1) {
			do_memcpy(replyBuffer, (void *) addr, wLength);
		} else {

#ifdef CONFIG_UHID_EEPROM_READBACK
			eeprom_read_block(replyBuffer, (void *) addr, wLength);
#endif
		}
		addr += wLength;
		return wLength;
	}
	return 0;
}

uchar usbFunctionWrite(uchar *data, uchar len)
{
	wLength -= len;

	if (target == 2) {
#ifdef CONFIG_UHID_EEPROM
		eeprom_write_block(data, (void *) addr, len);
#endif
		addr += len;
	} else {

		do {
			if ((addr & (SPM_PAGESIZE - 1)) == 0) {
				cli();
				boot_page_erase(addr); /* erase page */
				sei();
				boot_spm_busy_wait();
			}

			cli();
			boot_page_fill(addr, *((uint16_t *) data));
			sei();

			data+=2;
			addr+=2;
			len-=2;

			if ((addr & (SPM_PAGESIZE - 1)) == 0) {
				cli();
				boot_page_write(addr - 2);
				sei();
				boot_rww_enable_safe();
			}
		} while (len);
	}
	return (wLength == 0);
}


/* We won't use antares startup to save a few bytes */
int main()
{
	initRunButton();

	GICR = (1 << IVCE) ;  /* enable change of interrupt vectors */
	GICR = (1 << IVSEL); /* move interrupts to boot flash section */

	DDRD |= (1<<CONFIG_USB_CFG_DMINUS_BIT) | (1<<CONFIG_USB_CFG_DMINUS_BIT);
	PORTD &= ~((1<<CONFIG_USB_CFG_DMINUS_BIT) | (1<<CONFIG_USB_CFG_DMINUS_BIT));
	_delay_ms(25);
	DDRD &= ~((1<<CONFIG_USB_CFG_DMINUS_BIT) | (1<<CONFIG_USB_CFG_DMINUS_BIT));

	sei();
  	usbInit();

	if (checkRunButton())
		leaveBootloader();

	while (1)
		usbPoll();
}
