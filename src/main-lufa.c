 #include <avr/io.h>
 #include <avr/wdt.h>
 #include <avr/power.h>
 #include <avr/interrupt.h>
 #include <stdbool.h>
 #include <string.h>
#include <uhid.h>
 #include <LUFA/Drivers/USB/USB.h>


 #ifndef GICR
 #define GICR    MCUCR
 #endif

 void reconnect()
 {
 	USB_Detach();
 	_delay_ms(50);
 	USB_Attach();
 }

int main(void)
{
	GICR = (1 << IVCE) ;  /* enable change of interrupt vectors */
	GICR = (1 << IVSEL); /* move interrupts to boot flash section */

	initRunButton();
	USB_Init();
	GlobalInterruptEnable();
	reconnect();

#ifdef CONFIG_RUN_BUTTON_ON_START
	if (checkRunButton())
		leaveBootloader();
#endif


	while (1) {
		USB_USBTask();
#ifdef	CONFIG_RUN_BUTTON_ON_LOOP
		if (checkRunButton())
			leaveBootloader();
#endif
	}
}


void EVENT_USB_Device_ConfigurationChanged(void)
{
   Endpoint_ConfigureEndpoint(UHID_IN_EPADDR, EP_TYPE_INTERRUPT, UHID_IN_EPSIZE, 1);
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
