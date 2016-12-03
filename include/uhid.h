#ifndef UISP_H
#define UISP_H


#if defined(CONFIG_RUN_BUTTON_ON_START) || defined(CONFIG_RUN_BUTTON_ON_LOOP)
#define initRunButton() CONFIG_RUN_BUTTON_PORT |= (1<<CONFIG_RUN_BUTTON_BIT);
#define checkRunButton() (CONFIG_RUN_BUTTON_PIN & (1<<CONFIG_RUN_BUTTON_BIT))
#else
#define initRunButton()
#define checkRunButton() (0)
#endif


#endif
