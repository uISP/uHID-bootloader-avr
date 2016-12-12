#include <avr/boot.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <uhid.h>

/* Make sure we have all the VUSB-style structures */

#ifndef USB_STACK_VUSB

typedef unsigned char uchar;
#define USB_NO_MSG -1
#define USBRQ_HID_SET_REPORT HID_REQ_SetReport
#define USBRQ_HID_GET_REPORT HID_REQ_GetReport

typedef union usbWord {
    unsigned    word;
    uchar       bytes[2];
} usbWord_t;


typedef struct usbRequest{
    uchar       bmRequestType;
    uchar       bRequest;
    usbWord_t   wValue;
    usbWord_t   wIndex;
    usbWord_t   wLength;
} usbRequest_t;

#endif


/* Variables  */

static addr_t addr;

#ifndef UHID_TINY
static char opStart;
static uint8_t  wLength;
#endif

static char     target;




#ifndef CONFIG_UHID_EEPROM_READBACK
#	define EEP_PART_NAME "eepromwo"
#else
#	define EEP_PART_NAME "eeprom"
#endif

static const PROGMEM struct deviceInfo devInfo = {
	.version = 1,
	.cpuFreq = F_CPU / 10000,

#ifdef CONFIG_UHID_EEPROM
	.numParts = 2,
#else
	.numParts = 1,
#endif

	.parts = {
		{SPM_PAGESIZE, UHID_LOAD_ADDRESS, IOBUFLEN,       "flash"  },
#ifdef CONFIG_UHID_EEPROM
		{SPM_PAGESIZE, E2END + 1,          IOBUFLEN,       EEP_PART_NAME }
#endif
	},
};
#define INF_SIZE ((sizeof(struct deviceInfo) + 2 * sizeof(struct partInfo)))


#ifdef UHID_TINY
	static uchar    replyBuffer[INF_SIZE];
#else
	static uchar    replyBuffer[IOBUFLEN+1];
#endif



static void (*nullVector)(void) __attribute__((__noreturn__));

static void leaveBootloader()
{
	cli();
	reconnect();
#ifdef USB_STACK_VUSB
	USB_INTR_ENABLE = 0;
	USB_INTR_CFG = 0;       /* also reset config bits */
#endif
	GICR = (1 << IVCE);     /* enable change of interrupt vectors */
	GICR = (0 << IVSEL);    /* move interrupts to application flash section */
/* We must go through a global function pointer variable instead of writing
 *  ((void (*)(void))0)();
 * because the compiler optimizes a constant 0 to "rcall 0" which is not
 * handled correctly by the assembler.
 */
	nullVector();
}


static void flash_write_word(uint16_t data)
{
	cli();
	boot_page_fill(addr, data);
	sei();

	addr+=2;

	if ((addr & (SPM_PAGESIZE - 1)) == 0) {
		addr_t prev = addr - 2;
		boot_page_erase(prev); /* erase page */
		boot_spm_busy_wait();
		cli();
		boot_page_write(prev);
		sei();
		boot_rww_enable_safe();
	}
}

static char flash_byte_pos;
usbWord_t flashTmp;
static void flash_write_byte(uchar byte)
{
	int pos = flash_byte_pos & 0x1;
	flashTmp.bytes[pos] = byte;
	if (pos)
		flash_write_word(flashTmp.word);

	flash_byte_pos++;
}

uchar usbFunctionWrite(uchar *data, uchar len)
{
#ifndef UHID_TINY
	wLength -= len;
	/* Discard reportId */
	if (opStart) {
#endif
		data++;
		len--;
#ifndef UHID_TINY
		opStart=0;
	}
#endif

	if (target == 3) {
#ifdef CONFIG_UHID_EEPROM
		eeprom_write_block(data, (void *) addr, len);
		addr += len;
#endif
	} else {
#ifndef UHID_TINY
		 while (len--) {
			flash_write_byte(*data++);
		};
#else
		flash_write_word(* (uint16_t *) data);
#endif
	}
#ifdef UHID_TINY
	return 1;
#else
	/* wLength < 1 takes 10 bytes less that wLength == 0 */
    return (wLength < 1);
#endif
}


uchar uhidGenerateReport(uchar *buf, uint8_t target)
{
	replyBuffer[0]=target;
	if (target==1) {
		addr = 0;
		do_memcpy(&replyBuffer[1], &devInfo, UISP_INFO_LEN);
		return UISP_INFO_LEN+1;
	} else if (target == 2) {
		do_memcpy(&replyBuffer[1], (void *) addr, IOBUFLEN);
	}
#ifdef CONFIG_UHID_EEPROM_READBACK
	else {
		eeprom_read_block(&replyBuffer[1], (void *) addr, IOBUFLEN);
	}
#endif
	addr += IOBUFLEN;
	return IOBUFLEN+1;
}


uchar   usbFunctionSetup(uchar data[8])
{
	usbRequest_t    *rq = (void *)data;
	/* Avoid pointer some pointer derefs, they eat up flash */
	target = rq->wValue.bytes[0];
#ifndef UHID_TINY
	opStart = 1;
	wLength = rq->wLength.bytes[0];
#endif

	uint8_t bRequest = rq->bRequest;
	if (bRequest == USBRQ_HID_SET_REPORT) {
		if (target==1)
			leaveBootloader();
		return USB_NO_MSG;
	} else if(bRequest == USBRQ_HID_GET_REPORT) {
		usbMsgPtr = replyBuffer;
		return uhidGenerateReport(replyBuffer, target);
	}
	return 0;
}
