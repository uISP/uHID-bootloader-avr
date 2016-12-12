 #include <avr/io.h>
 #include <avr/wdt.h>
 #include <avr/power.h>
 #include <avr/interrupt.h>
 #include <stdbool.h>
 #include <string.h>

 #include <LUFA/Drivers/USB/USB.h>


 #ifndef GICR
 #define GICR    MCUCR
 #endif

int main(void)
{
	GICR = (1 << IVCE) ;  /* enable change of interrupt vectors */
	GICR = (1 << IVSEL); /* move interrupts to boot flash section */

	USB_Init();
	GlobalInterruptEnable();

	for (;;)
	{
		USB_USBTask();
	}
}


void reconnect()
{
	USB_Detach();
	_delay_ms(50);
	USB_Attach();
}

void *usbMsgPtr;
#include "bootlogic.c"
void EVENT_USB_Device_ControlRequest(void)
{

	uint8_t type = USB_ControlRequest.bmRequestType;
	type &= (REQTYPE_CLASS | REQREC_INTERFACE);

	if (type == (REQTYPE_CLASS | REQREC_INTERFACE))
	{
		uint8_t ret = usbFunctionSetup((void *)&USB_ControlRequest);
		Endpoint_ClearSETUP();
		if (ret == 0xff) { /* USB_NO_MSG */
			Endpoint_Read_Control_Stream_LE(replyBuffer, IOBUFLEN+1);
			usbFunctionWrite(replyBuffer, IOBUFLEN+1);
			Endpoint_ClearIN();
		} else if (ret > 0) {
			Endpoint_Write_Control_Stream_LE(usbMsgPtr, ret);
			Endpoint_ClearOUT();
		}
	}
}
