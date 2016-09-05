#ifndef UISP_H
#define UISP_H

#ifdef CONFIG_RUN_BUTTON
#define initRunButton() do { DDRC &= ~(1<<1); PORTC |= (1<<1); } while(0)
#define checkRunButton() ((PINC & (1<<1)))
#else
#define initRunButton()
#define checkRunButton() (0)
#endif


#endif
