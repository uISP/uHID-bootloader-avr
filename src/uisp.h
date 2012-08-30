#ifndef UISP_H
#define UISP_H

#define run_released() (PINC & 0x2)
#define usr_released() (PINC & 0x1)


struct uisp_app
{
	char name[16];
	char version[16];
};


#endif
